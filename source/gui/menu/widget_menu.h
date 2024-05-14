#pragma once

namespace Tyr::Gui
{
enum class ControllerMode
{
    Disabled = 0,
    Referee,
    Robot,
};

class WidgetMenu
{
public:
    WidgetMenu();
    ~WidgetMenu() = default;
    void draw(Common::Vec2 _mouse_pos);

private:
    void drawTabBar();
    void drawControllerTab();

    void refBroadcast(Protos::Ssl::Gc::Referee_Command _command);
    void drawJoystick();

    ControllerMode m_controller_mode = ControllerMode::Disabled;

    RenderTexture m_joystick_texture;
    // UDP_connection for ref commands
    std::unique_ptr<Common::UdpServer> m_udp;

    Protos::Ssl::Gc::Referee m_ref_packet;

    uint32_t                          m_command_counter = 0;
    Protos::Ssl::Gc::Referee_Stage    m_stage           = Protos::Ssl::Gc::Referee_Stage_NORMAL_FIRST_HALF;
    Protos::Ssl::Gc::Referee_TeamInfo m_team_info;
    Common::Vec2                      m_mouse_pos;
    Common::Vec2                      m_clicked_mouse_pos = {0., 0.};
    Texture2D                         m_xbox_texture;
    Texture2D                         m_ps5_texture;
};

} // namespace Tyr::Gui
