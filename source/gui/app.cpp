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
    if (!Common::Services::initialize())
    {
        return false;
    }

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
    SetTargetFPS(60);
    InitWindow(width, height, "Tyr");
    SetTraceLogLevel(LOG_ALL);
    rlImGuiSetup(true);

    m_renderer    = std::make_unique<Renderer>(Common::Vec2(900.f, 693.f), 1.0f);
    m_config_menu = std::make_unique<ConfigMenu>();
    m_widget_menu = std::make_unique<WidgetMenu>();
    m_demo_menu   = std::make_unique<DemoMenu>();
    m_log_menu    = std::make_unique<LogMenu>();

    m_renderer->initialize();

    Common::logInfo(" Now it is time, lets rock...");
    return true;
}

int Application::shutdown()
{
    m_running = false;

    m_ai_thread.join();
    m_ref_thread.join();
    m_str_thread.join();

    rlImGuiShutdown();

    // Close window and OpenGL context
    CloseWindow();

    Common::Services::shutdown();

    return 0;
}

void Application::start()
{
    m_ai_thread  = std::thread(&Application::aiThreadEntry, this);
    m_ref_thread = std::thread(&Application::refereeThreadEntry, this);
    m_str_thread = std::thread(&Application::strategyThreadEntry, this);
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
    if (((main_window_width - 650.) * 0.77) >= main_window_height - 200.)
    {
        main_window_width = (main_window_height - 200.) / 0.77 + 650.;
    }
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
        m_renderer->draw(Common::debug().wrapper());
        m_log_menu->draw(Common::debug().wrapper());
        m_drawing_mutex.unlock();

        m_renderer->applyShader();
        ImGui::Image(&m_renderer->shader_rt.texture, ImGui::GetContentRegionAvail());
        ImGui::End();
    }
    // end ImGui Content
    m_config_menu->draw();
    m_widget_menu->draw(m_renderer->getMousePosition());
    m_demo_menu->draw();
    rlImGuiEnd();

    EndDrawing();
}

bool Application::shouldClose() const
{
    return WindowShouldClose();
}

void Application::aiThreadEntry()
{
    Common::Timer timer;

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        timer.start();

        while (m_running && !m_vision->camsReady())
            m_vision->receive();

        if (!m_vision->camsReady())
            continue;

        m_ai_mutex.lock();

        m_vision->process();
        m_ai->Process();

        m_ai_mutex.unlock();

        for (auto &sender : m_senders)
            sender->flush();

        m_drawing_mutex.lock();
        Common::debug().flip();
        m_drawing_mutex.unlock();

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
        std::span<char> received_strategy;
        if (m_strategy_udp->receiveRaw(&received_strategy))
        {
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
}

} // namespace Tyr::Gui