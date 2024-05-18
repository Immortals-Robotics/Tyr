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

    m_sender_hub = std::make_unique<Sender::Hub>();
    m_sender_hub->registerSender<Sender::Nrf>();
    m_sender_hub->registerSender<Sender::Grsim>();

    m_ai = std::make_unique<Soccer::Ai>();

    m_dumper = std::make_unique<Common::Dumper>();
    m_dumper->addEntry(Common::setting().raw_world_state_url, Common::setting().raw_world_state_db);
    m_dumper->addEntry(Common::setting().world_state_url, Common::setting().world_state_db);
    m_dumper->addEntry(Common::setting().debug_url, Common::setting().debug_db);
    m_dumper->addEntry(Common::setting().referee_state_url, Common::setting().referee_db);

    m_world_client   = std::make_unique<Common::NngClient>(Common::setting().world_state_url);
    m_raw_client     = std::make_unique<Common::NngClient>(Common::setting().raw_world_state_url);
    m_debug_client   = std::make_unique<Common::NngClient>(Common::setting().debug_url);
    m_referee_client = std::make_unique<Common::NngClient>(Common::setting().referee_state_url);

    SetTraceLogCallback(logCallback);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Tyr");
    SetTraceLogLevel(LOG_ALL);

    const std::filesystem::path icon_path = std::filesystem::path(DATA_DIR) / "immortals.png";
    SetWindowIcon(LoadImage(icon_path.string().c_str()));

    rlImGuiSetup(true);

    m_renderer    = std::make_unique<Renderer>();
    m_config_menu = std::make_unique<ConfigMenu>();
    m_widget_menu = std::make_unique<WidgetMenu>();
    m_demo_menu   = std::make_unique<DemoMenu>();
    m_log_menu    = std::make_unique<FooterMenu>();

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

    m_dump_thread.join();

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

    m_dump_thread = std::thread(&Application::dumpEntry, this);
}

void Application::update()
{
    receiveWorldStates();
    receiveDebug();
    receiveRefereeState();

    BeginDrawing();
    ClearBackground(DARKGRAY);

    rlImGuiBegin();

    ImGuiWindowFlags renderer_window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration |
                                             ImGuiWindowFlags_NoBackground;
    int main_window_height = GetScreenHeight();
    int main_window_width  = GetScreenWidth();
    ImGui::SetNextWindowPos(ImVec2(250., 0.));
    if (((main_window_width - 650.) * 0.77) >= main_window_height - 200.)
    {
        main_window_width = (main_window_height - 200.) / 0.77 + 650.;
    }

    ImGui::SetNextWindowSize(ImVec2(main_window_width - 650., (main_window_width - 650.) * 0.77));

    bool opened = true;
    if (!ImGui::Begin("Field", &opened, renderer_window_flags))
    {
        ImGui::End();
    }
    else
    {
        m_renderer->begin(Common::field());

        m_renderer->draw(Common::field());

        // TODO(mhmd): add an option for this
        if (m_demo_menu->getState() == LogState::None)
        {
            m_renderer->draw(m_referee_state, Common::field());
            if (1)
                m_renderer->draw(m_world_state);
            else
                m_renderer->draw(m_raw_world_state);
        }
        else
        {
            m_renderer->draw(m_demo_menu->worldStateFiltered());
            m_renderer->draw(m_demo_menu->refereeState(), Common::field());
        }

        if (m_demo_menu->getState() == LogState::None)
        {
            m_config_menu->feedDebug(m_debug_wrapper);
            m_renderer->draw(m_debug_wrapper, m_config_menu->nodeMap());
            m_log_menu->draw(m_debug_wrapper, m_config_menu->nodeMap());
        }
        else
        {
            m_config_menu->feedDebug(m_demo_menu->debugWrapper());
            m_renderer->draw(m_demo_menu->debugWrapper(), m_config_menu->nodeMap());
            m_log_menu->draw(m_demo_menu->debugWrapper(), m_config_menu->nodeMap());
        }

        m_renderer->end();

        ImGui::End();
    }

    m_config_menu->draw();

    if (m_config_menu->isNetworkDataUpdated() == InputCallbackType::VISION_PORT ||
        m_config_menu->isNetworkDataUpdated() == InputCallbackType::VISION_IP)
    {
        updated_address.ip = m_config_menu->getNetworkParam(InputCallbackType::VISION_IP);
        updated_address.port =
            static_cast<unsigned short>(std::stoi(m_config_menu->getNetworkParam(InputCallbackType::VISION_PORT)));
        m_config_menu->updateNetworkData();
    }

    m_widget_menu->draw(m_renderer->mousePosition());
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
    if (m_raw_client->receive(&pb_raw_state, nullptr, true))
        m_raw_world_state = Common::RawWorldState(pb_raw_state);

    Protos::Immortals::WorldState pb_state;
    if (m_world_client->receive(&pb_state, nullptr, true))
        m_world_state = Common::WorldState(pb_state);
}

void Application::receiveRefereeState()
{
    Protos::Immortals::RefereeState pb_raw_state;
    if (m_referee_client->receive(&pb_raw_state, nullptr, true))
        m_referee_state = Common::RefereeState(pb_raw_state);
}

void Application::receiveDebug()
{
    Protos::Immortals::Debug::Wrapper pb_wrapper;
    if (m_debug_client->receive(&pb_wrapper, nullptr, true))
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
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_vision_raw->process();

        m_vision_raw->publish();

        Common::logInfo("vision raw FPS: {:.2f}", 1.0 / timer.intervalSmooth());
    }
}

void Application::visionFilteredEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        if (!m_vision_filtered->receive())
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_vision_filtered->process();

        m_vision_filtered->publish();

        Common::logInfo("vision filtered FPS: {:.2f}", 1.0 / timer.intervalSmooth());
    }
}

void Application::aiEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        const bool world_received    = m_ai->receiveWorld();
        const bool referee_received  = m_ai->receiveReferee();
        const bool playbook_received = m_ai->receivePlayBook();

        if (!world_received)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_ai->process();

        m_ai->publishCommands();

        Common::debug().flush();

        Common::logInfo("AI FPS: {:.2f}", 1.0 / timer.intervalSmooth());
    }
}

void Application::senderEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        if (!m_sender_hub->receive())
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_sender_hub->send();

        Common::logInfo("sender FPS: {:.2f}", 1.0 / timer.intervalSmooth());
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

        if (!ref_received && !world_received)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_referee->process();
        m_referee->publish();
    }
}

void Application::dumpEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && (ImmortalsIsTheBest)) // Hope it lasts Forever...
    {
        if (!m_dumper->process())
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }
    }
}
} // namespace Tyr::Gui