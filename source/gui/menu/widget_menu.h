#pragma once

namespace Tyr::Gui
{
enum class ControllerMode
{
    DISABLED = 0,
    REF,
    ROBOT
};

class WidgetMenu
{
private:

    void drawTabBar();
    void drawControllerTab();

    void refBroadcast(Protos::Referee_Command _command);
    void drawJoystick();
    
    int  m_controller_mode = static_cast<int>(ControllerMode::DISABLED);
    RenderTexture m_lstick_texture, m_rstick_texture, m_buttons_texture;
     // UDP_connection for ref commands
    Common::NetworkAddress             m_address;
    std::unique_ptr<Common::UdpServer> m_udp;

    std::unique_ptr<Protos::Referee> m_ref_packet;

    uint32_t m_command_counter = 0;
    Protos::Referee_Stage m_stage = Protos::Referee_Stage::Referee_Stage_NORMAL_FIRST_HALF;
    Protos::Referee_TeamInfo m_team_info;
    Common::Vec2 m_mouse_pos;
    Common::Vec2 m_clicked_mouse_pos = {0.,0.};
public:
    WidgetMenu();
    ~WidgetMenu() = default;
    void draw(Common::Vec2 _mouse_pos);
};

} // namespace Tyr::Gui
