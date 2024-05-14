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

    void refBroadcast(Protos::Ssl::Gc::Referee_Command _command);
    void drawJoystick();

    int           m_controller_mode = static_cast<int>(ControllerMode::DISABLED);
    RenderTexture m_joystick_texture;
    // UDP_connection for ref commands
    Common::NetworkAddress             m_address;
    std::unique_ptr<Common::UdpServer> m_udp;

    Protos::Ssl::Gc::Referee m_ref_packet;

    uint32_t                          m_command_counter = 0;
    Protos::Ssl::Gc::Referee_Stage    m_stage           = Protos::Ssl::Gc::Referee_Stage_NORMAL_FIRST_HALF;
    Protos::Ssl::Gc::Referee_TeamInfo m_team_info;
    Common::Vec2                      m_mouse_pos;
    Common::Vec2                      m_clicked_mouse_pos = {0., 0.};
    Texture2D                         m_xbox_texture;
    Texture2D                         m_ps5_texture;

public:
    WidgetMenu();
    ~WidgetMenu() = default;
    void draw(Common::Vec2 _mouse_pos);
};

} // namespace Tyr::Gui
