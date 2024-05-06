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

void Application::init(const int width, const int height)
{
    Common::Services::initialize();

    SetTraceLogCallback(logCallback);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Tyr");
    SetTraceLogLevel(LOG_WARNING);
    rlImGuiSetup(true);

    renderer    = std::make_unique<Renderer>(Common::Vec2(900.f, 693.f), 4.0f);
    config_menu = std::make_unique<ConfigMenu>();
    widget_menu = std::make_unique<WidgetMenu>();

    udp_client          = std::make_unique<Common::UdpClient>(Common::setting().vision_address);
    udp_client_drawings = std::make_unique<Common::UdpClient>(Common::NetworkAddress{"127.0.0.1", 10066});
    renderer->init();

    vision_thread  = std::thread(&Application::receiveVision, this);
    drawing_thread = std::thread(&Application::receiveDrawings, this);
}

int Application::shutdown()
{
    Common::Services::shutdown();

    running = false;
    vision_thread.join();
    drawing_thread.join();

    rlImGuiShutdown();

    // Close window and OpenGL context
    CloseWindow();

    return 0;
}

void Application::update()
{
    BeginDrawing();
    ClearBackground(DARKGRAY);

    // start ImGui Conent
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
        renderer->draw(Common::field());

        vision_mutex.lock();
        renderer->draw(Common::rawWorldState());
        vision_mutex.unlock();

        if (config_menu->isNetworkDataUpdated() == InputCallbackType::VISION_PORT ||
            config_menu->isNetworkDataUpdated() == InputCallbackType::VISION_IP)
        {
            updated_address.ip = config_menu->getNetworkParam(InputCallbackType::VISION_IP);
            updated_address.port =
                static_cast<unsigned short>(std::stoi(config_menu->getNetworkParam(InputCallbackType::VISION_PORT)));
            config_menu->updateNetworkData();
            udp_client->Update(updated_address);
        }

        drawing_mutex.lock();
        renderer->drawShapesUdp(debug_packet.draw());
        drawing_mutex.unlock();
        renderer->applyShader();
        // Common::logDebug("AV {}  {} pos {} {}", ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y,
        // main_window_width - 650., main_window_height * 0.8);
        ImGui::Image(&renderer->shaderVisualizationTexture.texture, ImGui::GetContentRegionAvail());
        ImGui::End();
    }
    // end ImGui Content
    config_menu->draw();
    widget_menu->draw(renderer->getMousePosition());
    rlImGuiEnd();

    EndDrawing();
}

bool Application::shouldClose() const
{
    return WindowShouldClose();
}

void Application::receiveVision()
{
    std::map<uint32_t, Protos::SSL_DetectionFrame> frame_map;

    while (running)
    {
        Protos::SSL_WrapperPacket tmp_ssl_packet;
        if (!udp_client->receive(&tmp_ssl_packet) || !tmp_ssl_packet.has_detection())
            continue;

        auto detection                   = tmp_ssl_packet.detection();
        frame_map[detection.camera_id()] = tmp_ssl_packet.detection();

        Protos::SSL_DetectionFrame merged_frame;
        for (const auto &detection : frame_map)
        {
            merged_frame.MergeFrom(detection.second);
        }

        vision_mutex.lock();
        // TODO: this will be moved to vision
        Common::rawWorldState() = Common::RawWorldState(merged_frame);
        vision_mutex.unlock();
    }
};

void Application::receiveDrawings()
{
    while (running)
    {
        Protos::Immortals::Debug::Wrapper tmp_debug_packet{};
        if (udp_client_drawings->receive(&tmp_debug_packet))
        {
            drawing_mutex.lock();
            std::swap(debug_packet, tmp_debug_packet);
            drawing_mutex.unlock();
        }
    }
};

} // namespace Tyr::Gui