#include "app.h"

namespace Tyr::Gui
{

static void logCallback(const int msg_type, const char *const text, va_list args)
{
    static constexpr int kBufferSize = 1024;
    char                 buffer[kBufferSize];

    const int size = std::vsnprintf(buffer, kBufferSize, text, args);

    const std::string_view formatted(buffer, size);

    switch (msg_type)
    {
    case LOG_INFO:
        Common::logInfo("{}", formatted);
        break;
    case LOG_ERROR:
        Common::logError("{}", formatted);
        break;
    case LOG_WARNING:
        Common::logWarning("{}", formatted);
        break;
    case LOG_DEBUG:
        Common::logDebug("{}", formatted);
        break;
    default:
        break;
    }
}

bool Application::initialize(const int width, const int height)
{
    Common::Services::initialize();

    if (!ImmortalsIsTheBest)
    {
        Common::logCritical("Immortals is not the best SSL team anymore.");
        Common::logCritical("Shutting down the system...");

        return false;
    }

    Common::logInfo(" Connecting to RefereeBox server at {} on port : {}", Common::setting().referee_address.port,
                    Common::setting().referee_address.port);
    m_referee = std::make_unique<Referee::Referee>();
    if (m_referee->connect())
    {
        Common::logInfo("Connected to RefBox successfully :)");
    }
    else
    {
        Common::logError("Hey you! Put the LAN cable back in its socket, or ...");
        return false;
    }

    Common::logInfo("Connecting to Vision server at {} on port: {}", Common::setting().vision_address.ip,
                    Common::setting().vision_address.port);
    m_vision = std::make_unique<Vision::Vision>();
    if (m_vision->isConnected())
    {
        Common::logInfo("Connected to Vision successfully :)");
    }
    else
    {
        Common::logError("Hey you! Put the LAN cable back in its socket, or ...");
        return false;
    }

    m_strategy_udp = std::make_unique<Common::UdpClient>(Common::setting().strategy_address);

    m_senders.push_back(std::make_unique<Sender::Nrf>());
    m_senders.push_back(std::make_unique<Sender::Grsim>(Common::setting().grsim_address));

    m_ai = std::make_unique<Soccer::Ai>(m_senders);

    SetTraceLogCallback(logCallback);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Tyr");
    SetTraceLogLevel(LOG_WARNING);
    rlImGuiSetup(true);

    m_renderer    = std::make_unique<Renderer>(Common::Vec2(900.f, 693.f), 4.0f);
    m_config_menu = std::make_unique<ConfigMenu>();
    m_widget_menu = std::make_unique<WidgetMenu>();

    udp_client_drawings = std::make_unique<Common::UdpClient>(Common::NetworkAddress{"127.0.0.1", 10066});
    m_renderer->initialize();

    Common::logInfo(" Now it is time, lets rock...");
    return true;
}

int Application::shutdown()
{
    m_running = false;

    m_drawing_thread.join();
    m_ai_thread.join();
    m_ref_thread.join();
    m_str_thread.join();

    Common::Services::shutdown();

    rlImGuiShutdown();

    // Close window and OpenGL context
    CloseWindow();

    return 0;
}

void Application::start()
{
    m_drawing_thread = std::thread(&Application::receiveDrawings, this);
    m_ai_thread      = std::thread(&Application::aiThreadEntry, this);
    m_ref_thread     = std::thread(&Application::refereeThreadEntry, this);
    m_str_thread     = std::thread(&Application::strategyThreadEntry, this);
}

void Application::update()
{
    BeginDrawing();
    ClearBackground(DARKGRAY);

    rlImGuiBegin();

    static bool  opened = true, opened2 = true;
    Common::Vec2 margin = Common::Vec2(30, 30) * 2;
    Common::Vec2 wSize  = Common::Vec2(900.f, 700.f) + margin;
    // TODO: draw gui
    ImGuiWindowFlags renderer_window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;
    auto main_window_height = GetScreenHeight();
    auto main_window_width  = GetScreenWidth();
    ImGui::SetNextWindowPos(ImVec2(250., 0.));
    ImGui::SetNextWindowSize(ImVec2(main_window_width - 650., (main_window_width - 650.) * 0.77));

    if (!ImGui::Begin("Field", &opened, renderer_window_flags))
    {
        ImGui::End();
    }
    else
    {
        m_renderer->draw(Common::field());

        m_ai_mutex.lock();
        // TODO(mhmd): add an option for this
        if (1)
            m_renderer->draw(Common::worldState());
        else
            m_renderer->draw(Common::rawWorldState());
        m_ai_mutex.unlock();

        if (m_config_menu->isNetworkDataUpdated() == InputCallbackType::VISION_PORT ||
            m_config_menu->isNetworkDataUpdated() == InputCallbackType::VISION_IP)
        {
            updated_address.ip = m_config_menu->getNetworkParam(InputCallbackType::VISION_IP);
            updated_address.port =
                static_cast<unsigned short>(std::stoi(m_config_menu->getNetworkParam(InputCallbackType::VISION_PORT)));
            m_config_menu->updateNetworkData();
        }

        m_drawing_mutex.lock();
        m_renderer->drawShapesUdp(debug_packet.draw());
        m_drawing_mutex.unlock();
        
        m_renderer->applyShader();
        // Common::logDebug("AV {}  {} pos {} {}", ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y,
        // main_window_width - 650., main_window_height * 0.8);
        ImGui::Image(&m_renderer->shader_rt.texture, ImGui::GetContentRegionAvail());
        ImGui::End();
    }
    // end ImGui Content
    m_config_menu->draw();
    m_widget_menu->draw(m_renderer->getMousePosition());
    rlImGuiEnd();

    EndDrawing();
}

bool Application::shouldClose() const
{
    return WindowShouldClose();
}

void Application::receiveDrawings()
{
    while (m_running)
    {
        Protos::Immortals::Debug::Wrapper tmp_debug_packet{};
        if (udp_client_drawings->receive(&tmp_debug_packet))
        {
            Common::logInfo("received {} debug draws from {}:{}", tmp_debug_packet.draw_size(),
                            udp_client_drawings->getLastReceiveEndpoint().address().to_string(),
                            udp_client_drawings->getLastReceiveEndpoint().port());

            m_drawing_mutex.lock();
            std::swap(debug_packet, tmp_debug_packet);
            m_drawing_mutex.unlock();
        }
    }
};

void Application::aiThreadEntry()
{
    Common::Timer timer;

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        timer.start();

        m_vision->receive();

        m_ai_mutex.lock();

        m_vision->process();
        m_ai->Process();

        for (auto &sender : m_senders)
            sender->flush();

        Common::debug().broadcast();

        m_ai_mutex.unlock();
        Common::logInfo("FPS: {}", 1.0 / timer.interval());
    }
}

void Application::refereeThreadEntry()
{
    while (m_running && (ImmortalsIsTheBest)) // Hope it lasts Forever...
    {
        if (m_referee->receive())
        {
            m_ai_mutex.lock();
            m_referee->process();
            m_ai_mutex.unlock();
        }
    }
}

void Application::strategyThreadEntry()
{
    while (m_running && (ImmortalsIsTheBest)) // Hope it lasts Forever...
    {
        auto received_strategy = m_strategy_udp->receiveRaw();

        if (received_strategy.size() > 11)
        {
            const auto receive_endpoint = m_strategy_udp->getLastReceiveEndpoint();
            Common::logInfo("Received \"strategy.ims\" with size: {} KB, from {} on port {}",
                            float(received_strategy.size()) / 1000.0f, receive_endpoint.address().to_string(),
                            receive_endpoint.port());

            m_ai_mutex.lock();
            m_ai->read_playBook_str(received_strategy);
            m_ai_mutex.unlock();

            const std::filesystem::path strategy_path =
                std::filesystem::path{DATA_DIR} / std::filesystem::path{"strategy.ims"};

            std::ofstream strategyFile(strategy_path, std::ios::out | std::ios::binary);
            strategyFile.write(received_strategy.data(), received_strategy.size());
            strategyFile.close();
        }
        else
        {
            Common::logWarning("Invalid \"strategy.ims\" received with size: {}", received_strategy.size());
        }
    }
}

} // namespace Tyr::Gui