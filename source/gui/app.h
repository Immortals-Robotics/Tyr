#pragma once

#define ImmortalsIsTheBest true

#include "menu/config_menu.h"
#include "menu/widget_menu.h"
#include "menu/demo_menu.h"
#include "renderer/renderer.h"

namespace Tyr::Gui
{
class Application
{
public:
    bool initialize(int width = 1500, int height = 900);
    int  shutdown();

    void start();

    void update();

    bool shouldClose() const;

private:
    std::unique_ptr<Renderer>   m_renderer;
    std::unique_ptr<ConfigMenu> m_config_menu;
    std::unique_ptr<WidgetMenu> m_widget_menu;
    std::unique_ptr<DemoMenu>   m_demo_menu;

    std::unique_ptr<Common::UdpClient> m_strategy_udp;

    Common::NetworkAddress updated_address;

    std::atomic<bool> m_running = true;

    std::mutex m_ai_mutex;
    std::mutex m_drawing_mutex;

    std::thread m_ai_thread;
    std::thread m_ref_thread;
    std::thread m_str_thread;

    std::unique_ptr<Referee::Referee> m_referee;

    std::unique_ptr<Vision::Vision>               m_vision;
    std::vector<std::unique_ptr<Sender::ISender>> m_senders;
    std::unique_ptr<Soccer::Ai>                   m_ai;

    void aiThreadEntry();
    void refereeThreadEntry();
    void strategyThreadEntry();
};
} // namespace Tyr::Gui