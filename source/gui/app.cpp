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
    case LOG_TRACE:
        Common::logTrace("{}", formatted);
        break;
    case LOG_DEBUG:
        Common::logDebug("{}", formatted);
        break;
    case LOG_INFO:
        Common::logInfo("{}", formatted);
        break;
    case LOG_WARNING:
        Common::logWarning("{}", formatted);
        break;
    case LOG_ERROR:
        Common::logError("{}", formatted);
        break;
    case LOG_FATAL:
        Common::logCritical("{}", formatted);
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
    m_vision_raw = std::make_unique<Vision::Raw>();
    if (m_vision_raw->isConnected())
    {
        Common::logInfo("Connected to Vision successfully :)");
    }
    else
    {
        Common::logError("Hey you! Put the LAN cable back in its socket, or ...");
        return false;
    }

    m_vision_filtered = std::make_unique<Vision::Filtered>();

    m_strategy_udp = std::make_unique<Common::UdpClient>(Common::setting().strategy_address);

    m_senders.push_back(std::make_unique<Sender::Nrf>());
    m_senders.push_back(std::make_unique<Sender::Grsim>());

    m_ai = std::make_unique<Soccer::Ai>();

    m_world_client = std::make_unique<Common::NngClient>(Common::setting().world_state_url);
    m_raw_client   = std::make_unique<Common::NngClient>(Common::setting().raw_world_state_url);
    m_debug_client = std::make_unique<Common::NngClient>(Common::setting().debug_url);

    SetTraceLogCallback(logCallback);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Tyr");
    SetTraceLogLevel(LOG_ALL);

    const std::filesystem::path icon_path = std::filesystem::path(DATA_DIR) / "immortals.png";
    SetWindowIcon(LoadImage(icon_path.string().c_str()));

    rlImGuiSetup(true);

    m_renderer    = std::make_unique<Renderer>(Common::Vec2(900.f, 693.f), 4.0f);
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

    m_vision_raw_thread.join();
    m_vision_filtered_thread.join();

    m_ai_thread.join();
    m_sender_thread.join();
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
    m_vision_raw_thread      = std::thread(&Application::visionRawEntry, this);
    m_vision_filtered_thread = std::thread(&Application::visionFilteredEntry, this);

    m_ai_thread     = std::thread(&Application::aiEntry, this);
    m_sender_thread = std::thread(&Application::senderEntry, this);
    m_ref_thread    = std::thread(&Application::refereeEntry, this);
    m_str_thread    = std::thread(&Application::strategyEntry, this);
}

void Application::update()
{
    receiveWorldStates();
    receiveDebug();

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

        // TODO(mhmd): add an option for this
        if (1)
            m_renderer->draw(m_world_state);
        else
            m_renderer->draw(m_raw_world_state);

        if (m_config_menu->isNetworkDataUpdated() == InputCallbackType::VISION_PORT ||
            m_config_menu->isNetworkDataUpdated() == InputCallbackType::VISION_IP)
        {
            updated_address.ip = m_config_menu->getNetworkParam(InputCallbackType::VISION_IP);
            updated_address.port =
                static_cast<unsigned short>(std::stoi(m_config_menu->getNetworkParam(InputCallbackType::VISION_PORT)));
            m_config_menu->updateNetworkData();
        }

        m_renderer->draw(m_debug_wrapper);
        m_log_menu->draw(m_debug_wrapper);

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

void Application::receiveWorldStates()
{
    Protos::Immortals::RawWorldState pb_raw_state;
    if (m_raw_client->receive(&pb_raw_state))
        m_raw_world_state = Common::RawWorldState(pb_raw_state);

    Protos::Immortals::WorldState pb_state;
    if (m_world_client->receive(&pb_state))
        m_world_state = Common::WorldState(pb_state);
}

void Application::receiveDebug()
{
    Protos::Immortals::Debug::Wrapper pb_wrapper;
    if (m_debug_client->receive(&pb_wrapper))
        m_debug_wrapper = Common::Debug::Wrapper(pb_wrapper);
}

void Application::visionRawEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        m_vision_raw->receive();

        if (!m_vision_raw->camsReady())
            continue;

        m_vision_raw->process();

        m_vision_raw->publish();
        m_vision_raw->store();

        Common::logInfo("vision raw FPS: {}", 1.0 / timer.interval());
    }
}

void Application::visionFilteredEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        if (!m_vision_filtered->receive())
            continue;

        m_vision_filtered->process();

        m_vision_filtered->publish();
        m_vision_filtered->store();

        Common::logInfo("vision filtered FPS: {}", 1.0 / timer.interval());
    }
}

void Application::aiEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        const bool world_received   = m_ai->receiveWorld();
        const bool referee_received = m_ai->receiveReferee();

        if (!world_received)
            continue;

        m_ai->process();

        m_ai->publishCommands();

        Common::debug().flush();

        Common::logInfo("AI FPS: {}", 1.0 / timer.interval());
    }
}

void Application::senderEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        bool any = false;
        for (const auto &sender : m_senders)
        {
            if (!sender->receive())
                continue;

            any = true;
            sender->send();
        }

        if (any)
            Common::logInfo("sender FPS: {}", 1.0 / timer.interval());
    }
}

void Application::refereeEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && (ImmortalsIsTheBest)) // Hope it lasts Forever...
    {
        const bool ref_received   = m_referee->receiveRef();
        const bool world_received = m_referee->receiveWorld();
        if (ref_received || world_received)
        {
            m_referee->process();
            m_referee->publish();

            Common::logInfo("referee FPS: {}", 1.0 / timer.interval());
        }
    }
}

void Application::strategyEntry()
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