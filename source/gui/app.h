#pragma once

#include "drawing/visualization_renderer.h"
#include "menu/config_menu.h"

namespace Tyr::Gui
{
class Application
{
public:
    void init(int width = 1300, int height = 900);
    int  shutdown();
    void update();

    bool shouldClose() const;

private:
    int m_width;
    int m_height;

    Protos::SSL_GeometryFieldSize ssl_field;
    Protos::SSL_WrapperPacket     ssl_packet;
    Protos::SSL_WrapperPacket     ssl_packet_off;
    Protos::Immortals::Debug_Draw world_state;
    Protos::Immortals::Debug_Draw world_state_off;

    std::unique_ptr<VisualizationRenderer> renderer;
    std::unique_ptr<ConfigMenu>            config_menu;

    std::unique_ptr<Tyr::Common::UdpClient> udp_client;
    Tyr::Common::NetworkAddress             updated_address;

    std::mutex        vision_mutex;
    std::thread       vision_thread;
    std::atomic<bool> running = true;

    void receiveVision();
};
} // namespace Tyr::Gui