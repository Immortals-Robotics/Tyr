#pragma once

#define ImmortalsIsTheBest true

#include "menu/config_menu.h"
#include "menu/demo_menu.h"
#include "menu/footer_menu.h"
#include "menu/widget_menu.h"
#include "renderer/renderer.h"
namespace Tyr::Gui
{
class Application
{
public:
    bool initialize(int t_width = 1500, int t_height = 900);
    int  shutdown();

    void start();

    void update();

    bool shouldClose() const;

private:
    ImGuiID m_root_dockspace     = 0;
    bool    m_layout_initialized = false;

    std::unique_ptr<Renderer>   m_renderer;
    std::unique_ptr<ConfigMenu> m_config_menu;
    std::unique_ptr<WidgetMenu> m_widget_menu;
    std::unique_ptr<DemoMenu>   m_demo_menu;
    std::unique_ptr<FooterMenu> m_footer_menu;

    std::atomic<bool> m_running = true;

    std::thread m_vision_raw_thread;
    std::thread m_vision_filtered_thread;
    std::thread m_ai_thread;
    std::thread m_sender_thread;
    std::thread m_ref_thread;
    std::thread m_dump_thread;

    std::unique_ptr<Referee::Referee> m_referee;

    std::unique_ptr<Vision::Raw>      m_vision_raw;
    std::unique_ptr<Vision::Filtered> m_vision_filtered;

    std::unique_ptr<Sender::Hub> m_sender_hub;
    std::unique_ptr<Soccer::Ai>  m_ai;

    std::unique_ptr<Common::Dumper> m_dumper;

    std::unique_ptr<Common::NngClient> m_world_client;
    std::unique_ptr<Common::NngClient> m_raw_client;
    std::unique_ptr<Common::NngClient> m_debug_client;
    std::unique_ptr<Common::NngClient> m_referee_client;

    Common::WorldState    m_world_state;
    Common::RawWorldState m_raw_world_state;

    Common::Debug::Wrapper m_debug_wrapper;

    Common::RefereeState m_referee_state;

    void resetLayout();

    void receiveWorldStates();
    void receiveDebug();
    void receiveRefereeState();

    void visionRawEntry() const;
    void visionFilteredEntry() const;

    void aiEntry() const;
    void senderEntry() const;
    void refereeEntry() const;

    void dumpEntry() const;

    bool live() const
    {
        return m_demo_menu->getState() == LogState::Live;
    }

    const Common::WorldState &worldState() const
    {
        return live() ? m_world_state : m_demo_menu->worldState();
    }

    const Common::Debug::Wrapper &debugWrapper() const
    {
        return live() ? m_debug_wrapper : m_demo_menu->debugWrapper();
    }
};
} // namespace Tyr::Gui
