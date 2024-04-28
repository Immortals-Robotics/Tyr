#include "app.h"

namespace Tyr::Gui
{
void Application::init(const int width, const int height)
{
    m_width  = width;
    m_height = height;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(m_width, m_height, "Immortals SSL");
    SetTraceLogLevel(LOG_WARNING);
    rlImGuiSetup(true);

    renderer    = std::make_unique<Renderer>(ImVec2(900.f, 700.f), 4.0f);
    config_menu = std::make_unique<ConfigMenu>(ImVec2(m_width, m_height));

    ssl_field.set_field_length(12000);
    ssl_field.set_field_width(9000);
    ssl_field.set_goal_width(1800);
    ssl_field.set_goal_depth(180);

    ssl_field.set_boundary_width(300);
    ssl_field.set_center_circle_radius(500);
    ssl_field.set_penalty_area_depth(1800);
    ssl_field.set_penalty_area_width(3600);

    udp_client = std::make_unique<Tyr::Common::UdpClient>(Common::NetworkAddress{"224.5.23.2", 10006});

    renderer->init();

    vision_thread = std::thread(&Application::receiveVision, this);
}

int Application::shutdown()
{
    running = false;
    vision_thread.join();

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

    static bool opened = true, opened2 = true;
    ImVec2      margin = ImVec2(30, 30) * 2;
    ImVec2      wSize  = ImVec2(900.f, 700.f) + margin;
    // TODO: draw gui
    ImGui::SetNextWindowSize(wSize, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Field", &opened))
    {
        ImGui::End();
    }
    else
    {
        renderer->DrawField(ssl_field);
        vision_mutex.lock();
        renderer->DrawRobots(ssl_packet.detection().robots_blue(), Tyr::Common::TeamColor::Blue);
        renderer->DrawRobots(ssl_packet.detection().robots_yellow(), Tyr::Common::TeamColor::Yellow);
        renderer->DrawBalls(ssl_packet.detection().balls());
        if (config_menu->IsNetworkDataUpdated() == NetworkInput::VisionPort ||
            config_menu->IsNetworkDataUpdated() == NetworkInput::VisionIp)
        {
            updated_address.ip = config_menu->GetNetworkParam(NetworkInput::VisionIp);
            updated_address.port =
                static_cast<unsigned short>(std::stoi(config_menu->GetNetworkParam(NetworkInput::VisionPort)));
            config_menu->UpdateNetworkData();
            udp_client->Update(updated_address);
        }
        vision_mutex.unlock();

        renderer->ApplyShader();
        rlImGuiImageRenderTextureFit(&renderer->shaderVisualizationTexture, true);
        ImGui::End();
    }
    // end ImGui Content
    config_menu->Draw();

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

} // namespace Tyr::Gui