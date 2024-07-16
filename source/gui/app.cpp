#include "app.h"

namespace Tyr::Gui
{
static void logCallback(const int t_msg_type, const char *const t_text, va_list t_args)
{
    static constexpr int kBufferSize = 1024;
    char                 buffer[kBufferSize];

    const int size = std::vsnprintf(buffer, kBufferSize, t_text, t_args);

    const std::string_view formatted(buffer, size);

    switch (t_msg_type)
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

bool Application::initialize(const int t_width, const int t_height)
{
    const Common::Services::Params params{.t_config_path = std::filesystem::path{DATA_DIR} / "config.toml",
                                          .t_db_path     = std::filesystem::path{LOG_DIR} / "db"};
    if (!Common::Services::initialize(params))
    {
        return false;
    }

    if (!Common::config().common.immortals_is_the_best_team)
    {
        Common::logCritical("Immortals is not the best SSL team anymore.");
        Common::logCritical("Shutting down the system...");

        return false;
    }

    Common::logInfo(" Connecting to RefereeBox server at {} on port : {}",
                    Common::config().network.referee_address.port, Common::config().network.referee_address.port);
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

    Common::logInfo("Connecting to Vision server at {}", Common::config().network.vision_address);
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
    m_sender_hub->registerSender<Sender::Simulator>();

    m_ai = std::make_unique<Soccer::Ai>();

    m_dumper = std::make_unique<Common::Dumper>();
    m_dumper->addEntry(Common::config().network.raw_world_state_url, Common::config().network.raw_world_state_db);
    m_dumper->addEntry(Common::config().network.world_state_url, Common::config().network.world_state_db);
    m_dumper->addEntry(Common::config().network.debug_url, Common::config().network.debug_db);
    m_dumper->addEntry(Common::config().network.referee_state_url, Common::config().network.referee_db);
    m_dumper->addEntry(Common::config().network.soccer_state_url, Common::config().network.soccer_db);

    m_world_client   = std::make_unique<Common::NngClient>(Common::config().network.world_state_url);
    m_raw_client     = std::make_unique<Common::NngClient>(Common::config().network.raw_world_state_url);
    m_debug_client   = std::make_unique<Common::NngClient>(Common::config().network.debug_url);
    m_referee_client = std::make_unique<Common::NngClient>(Common::config().network.referee_state_url);
    m_soccer_client  = std::make_unique<Common::NngClient>(Common::config().network.soccer_state_url);

    SetTraceLogCallback(logCallback);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(t_width, t_height, "Tyr");
    SetTraceLogLevel(LOG_ALL);

    const std::filesystem::path icon_path = std::filesystem::path(DATA_DIR) / "immortals.png";
    SetWindowIcon(LoadImage(icon_path.string().c_str()));

    rlImGuiSetup(true);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiTheme::ApplyTheme(ImGuiTheme::ImGuiTheme_SoDark_AccentRed);

    m_renderer        = std::make_unique<Renderer>();
    m_soccer_menu     = std::make_unique<SoccerMenu>();
    m_config_menu     = std::make_unique<ConfigMenu>();
    m_controller_menu = std::make_unique<ControllerMenu>();
    m_demo_menu       = std::make_unique<DemoMenu>();
    m_filter_menu     = std::make_unique<FilterMenu>();
    m_log_menu        = std::make_unique<LogMenu>();
    m_plot_menu       = std::make_unique<PlotMenu>();
    m_status_bar      = std::make_unique<StatusBar>();

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
    receiveSoccerState();

    BeginDrawing();
    ClearBackground(DARKGRAY);

    rlImGuiBegin();

    m_root_dockspace = ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
    if (!m_layout_initialized)
    {
        resetLayout();
        m_layout_initialized = true;
    }

    if (ImGui::Begin("Status"))
    {
        m_status_bar->draw(debugWrapper());
    }
    ImGui::End();

    if (ImGui::Begin("Field"))
    {
        // the actual draw happens at the end
    }
    ImGui::End();

    m_filter_menu->feedDebug(debugWrapper());

    if (ImGui::Begin("Log"))
    {
        m_log_menu->draw(debugWrapper(), m_filter_menu->map());
    }
    ImGui::End();

    if (ImGui::Begin("Plot"))
    {
        m_plot_menu->draw(worldState(), !live());
    }
    ImGui::End();

    if (ImGui::Begin("Soccer"))
    {
        m_soccer_menu->draw(soccerState());
    }
    ImGui::End();

    if (ImGui::Begin("Config"))
    {
        m_config_menu->draw();
    }
    ImGui::End();

    if (ImGui::Begin("Debug Filter"))
    {
        m_filter_menu->draw();
    }
    ImGui::End();

    if (ImGui::IsMouseClicked(0))
    {
        m_controller_menu->setMouseClickPos(m_renderer->mousePosition());
    }

    if (ImGui::Begin("Gamepad"))
    {
        m_controller_menu->draw();
    }
    ImGui::End();

    m_demo_menu->update();
    if (ImGui::Begin("Demo player"))
    {
        m_demo_menu->draw();
    }
    ImGui::End();

    rlImGuiEnd();

    // Draw field after imgui to appear on top
    const ImGuiWindow *field_window = ImGui::FindWindowByName("Field");
    if (!field_window->Hidden)
    {
        const Common::Rect field_rect{Common::Vec2{field_window->InnerRect.Min},
                                      Common::Vec2{field_window->InnerRect.Max}};

        m_renderer->begin(Common::field(), field_rect);

        m_renderer->draw(Common::field());

        m_renderer->draw(refereeState(), Common::field());
        m_renderer->draw(worldState());

        m_renderer->draw(debugWrapper(), m_filter_menu->map());

        m_renderer->end();
    }

    EndDrawing();
}

bool Application::shouldClose() const
{
    return WindowShouldClose();
}

void Application::resetLayout()
{
    ImGui::DockBuilderRemoveNode(m_root_dockspace);
    ImGui::DockBuilderAddNode(m_root_dockspace, ImGuiDockNodeFlags_PassthruCentralNode |
                                                    (ImGuiDockNodeFlags) ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(m_root_dockspace, ImGui::GetMainViewport()->Size);

    ImGuiID dockspace_main = m_root_dockspace;

    ImGuiID dockspace_down_bar =
        ImGui::DockBuilderSplitNode(dockspace_main, ImGuiDir_Down, 0.035f, nullptr, &dockspace_main);

    ImGuiID dockspace_down =
        ImGui::DockBuilderSplitNode(dockspace_main, ImGuiDir_Down, 0.25f, nullptr, &dockspace_main);

    ImGuiID dockspace_right =
        ImGui::DockBuilderSplitNode(dockspace_main, ImGuiDir_Right, 0.25f, nullptr, &dockspace_main);
    ImGuiID dockspace_left =
        ImGui::DockBuilderSplitNode(dockspace_main, ImGuiDir_Left, 0.25f, nullptr, &dockspace_main);

    ImGuiID dockspace_right_top;
    ImGuiID dockspace_right_down =
        ImGui::DockBuilderSplitNode(dockspace_right, ImGuiDir_Down, 0.5f, nullptr, &dockspace_right_top);

    ImGuiID dockspace_down_left;
    ImGuiID dockspace_down_right =
        ImGui::DockBuilderSplitNode(dockspace_down, ImGuiDir_Right, 0.5f, nullptr, &dockspace_down_left);

    ImGui::DockBuilderDockWindow("Field", dockspace_main);

    ImGui::DockBuilderDockWindow("Soccer", dockspace_left);
    ImGui::DockBuilderDockWindow("Config", dockspace_left);
    ImGui::DockBuilderDockWindow("Debug Filter", dockspace_left);

    ImGui::DockBuilderDockWindow("Gamepad", dockspace_right_top);
    ImGui::DockBuilderDockWindow("Demo player", dockspace_right_down);

    ImGui::DockBuilderDockWindow("Log", dockspace_down_left);
    ImGui::DockBuilderDockWindow("Plot", dockspace_down_right);

    ImGui::DockBuilderGetNode(dockspace_down_bar)
        ->SetLocalFlags(ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResizeX | ImGuiDockNodeFlags_NoResizeY |
                        ImGuiDockNodeFlags_NoDocking);
    ImGui::DockBuilderDockWindow("Status", dockspace_down_bar);

    ImGui::DockBuilderFinish(m_root_dockspace);
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
    Protos::Immortals::Referee::State pb_state;
    if (m_referee_client->receive(&pb_state, nullptr, true))
        m_referee_state = Common::Referee::State(pb_state);
}

void Application::receiveSoccerState()
{
    Protos::Immortals::Soccer::State pb_state;
    if (m_soccer_client->receive(&pb_state, nullptr, true))
        m_soccer_state = Common::Soccer::State(pb_state);
}

void Application::receiveDebug()
{
    Protos::Immortals::Debug::Wrapper pb_wrapper;
    if (m_debug_client->receive(&pb_wrapper, nullptr, true))
        m_debug_wrapper = Common::Debug::Wrapper(pb_wrapper);
}

void Application::visionRawEntry() const
{
    Common::Debug::setThreadName("VisionRaw");

    while (m_running && Common::config().common.immortals_is_the_best_team) // Hope it lasts Forever...
    {
        m_vision_raw->receive();

        if (!m_vision_raw->camsReady())
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_vision_raw->process();
        m_vision_raw->publish();
    }
}

void Application::visionFilteredEntry() const
{
    Common::Debug::setThreadName("VisionFiltered");

    Common::Timer interval_timer;
    interval_timer.start();

    while (m_running && Common::config().common.immortals_is_the_best_team) // Hope it lasts Forever...
    {
        const bool                  raw_received = m_vision_filtered->receiveRaw();
        [[maybe_unused]] const bool cmd_received = m_vision_filtered->receiveCmds();

        if (!raw_received)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        Common::Timer duration_timer;
        duration_timer.start();

        m_vision_filtered->process();
        m_vision_filtered->publish();

        Common::Debug::ExecutionTime execution_time;
        execution_time.duration = duration_timer.time();
        execution_time.interval = interval_timer.interval();

        Common::debug().reportExecutionTime("vision", execution_time);
    }
}

void Application::aiEntry() const
{
    Common::Debug::setThreadName("Ai");

    Common::Timer interval_timer;
    interval_timer.start();

    while (m_running && Common::config().common.immortals_is_the_best_team) // Hope it lasts Forever...
    {
        const bool world_received = m_ai->receiveWorld();
        m_ai->receiveReferee();
        m_ai->receivePlayBook();

        if (!world_received)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        Common::Timer duration_timer;
        duration_timer.start();

        m_ai->process();

        if (!m_ai->publishCommands())
        {
            Common::logWarning("Failed to publish commands");
        }

        if (!m_ai->publishState())
        {
            Common::logWarning("Failed to publish state");
        }

        Common::Debug::ExecutionTime execution_time;
        execution_time.duration = duration_timer.time();
        execution_time.interval = interval_timer.interval();

        Common::debug().reportExecutionTime("ai", execution_time);

        Common::debug().flush();
    }
}

void Application::senderEntry() const
{
    Common::Debug::setThreadName("Sender");

    while (m_running && Common::config().common.immortals_is_the_best_team) // Hope it lasts Forever...
    {
        if (!m_sender_hub->receive())
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_sender_hub->send();
    }
}

void Application::refereeEntry() const
{
    Common::Debug::setThreadName("Referee");

    while (m_running && (Common::config().common.immortals_is_the_best_team)) // Hope it lasts Forever...
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

void Application::dumpEntry() const
{
    Common::Debug::setThreadName("Dump");

    while (m_running && (Common::config().common.immortals_is_the_best_team)) // Hope it lasts Forever...
    {
        if (!m_dumper->process())
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}
} // namespace Tyr::Gui
