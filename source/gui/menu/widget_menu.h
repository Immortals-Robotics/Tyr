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

    void setMouseClickPos(const Common::Vec2 t_mouse_pos)
    {
        m_clicked_mouse_pos = t_mouse_pos;
    }

    void drawControllerTab();

private:
    void refBroadcast(Protos::Ssl::Gc::Referee_Command t_command);

    void drawJoystick();

    // UDP socket for ref commands
    std::unique_ptr<Common::UdpServer> m_udp;

    ControllerMode m_controller_mode = ControllerMode::Disabled;

    Protos::Ssl::Gc::Referee_Command m_last_command    = Protos::Ssl::Gc::Referee_Command_HALT;
    uint32_t                         m_command_counter = 0;

    Common::Vec2 m_clicked_mouse_pos = {0., 0.};

    RenderTexture m_joystick_texture;
    Texture2D     m_xbox_texture;
    Texture2D     m_ps5_texture;
};

} // namespace Tyr::Gui
