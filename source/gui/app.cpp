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

    renderer    = std::make_unique<Renderer>(Common::Vec2(900.f, 700.f), 4.0f);
    config_menu = std::make_unique<ConfigMenu>();

    ssl_field.set_field_length(12000);
    ssl_field.set_field_width(9000);
    ssl_field.set_goal_width(1800);
    ssl_field.set_goal_depth(180);

    ssl_field.set_boundary_width(300);
    ssl_field.set_center_circle_radius(500);
    ssl_field.set_penalty_area_depth(1800);
    ssl_field.set_penalty_area_width(3600);

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
    ImGui::SetNextWindowSize(ImVec2(wSize.x, wSize.y), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Field", &opened))
    {
        ImGui::End();
    }
    else
    {
        renderer->drawField(ssl_field);
        vision_mutex.lock();
        renderer->drawRobots(ssl_packet.detection().robots_blue(), Common::TeamColor::Blue);
        renderer->drawRobots(ssl_packet.detection().robots_yellow(), Common::TeamColor::Yellow);
        renderer->drawBalls(ssl_packet.detection().balls());
        if (config_menu->isNetworkDataUpdated() == InputCallbackType::VISION_PORT ||
            config_menu->isNetworkDataUpdated() == InputCallbackType::VISION_IP)
        {
            updated_address.ip = config_menu->getNetworkParam(InputCallbackType::VISION_IP);
            updated_address.port =
                static_cast<unsigned short>(std::stoi(config_menu->getNetworkParam(InputCallbackType::VISION_PORT)));
            config_menu->updateNetworkData();
            udp_client->Update(updated_address);
        }
        vision_mutex.unlock();
        drawing_mutex.lock();
        renderer->drawCirclesUdp(debug_packet.dbg_draw().circle());
        renderer->drawRectsUdp(debug_packet.dbg_draw().rect());
        renderer->drawPointsUdp(debug_packet.dbg_draw().point());
        renderer->drawLinesUdp(debug_packet.dbg_draw().line());
        drawing_mutex.unlock();

        renderer->applyShader();
        rlImGuiImageRenderTextureFit(&renderer->shaderVisualizationTexture, true);
        ImGui::End();
    }
    // end ImGui Content
    config_menu->draw();

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
    Protos::SSL_WrapperPacket                      tmp_ssl_packet;

    while (running)
    {
        udp_client->receive(&tmp_ssl_packet);

        if (tmp_ssl_packet.has_detection())
        {
            auto detection                   = tmp_ssl_packet.detection();
            frame_map[detection.camera_id()] = detection;
        }

        ssl_packet_off.clear_detection();
        for (auto detection : frame_map)
        {
            ssl_packet_off.mutable_detection()->MergeFrom(detection.second);
        }

        vision_mutex.lock();
        std::swap(ssl_packet, ssl_packet_off);
        vision_mutex.unlock();
    }
};

void Application::receiveDrawings()
{
    Protos::Immortals::Imm_DBG_wrapper tmp_drawing_packet;
    while (running)
    {
        udp_client_drawings->receive(&tmp_drawing_packet);
        if (tmp_drawing_packet.has_dbg_draw())
        {
            auto draw = tmp_drawing_packet.dbg_draw();
            // Common::logDebug("draw darim");
            debug_packet_off.clear_dbg_draw();
            debug_packet_off.mutable_dbg_draw()->CopyFrom(draw);
        }
        drawing_mutex.lock();
        std::swap(debug_packet, debug_packet_off);
        drawing_mutex.unlock();
    }
};

} // namespace Tyr::Gui