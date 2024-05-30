#pragma once

namespace Tyr::Gui
{
class ControllerMenu
{
public:
     ControllerMenu();
    ~ControllerMenu() = default;

    void setMouseClickPos(const Common::Vec2 t_mouse_pos)
    {
        m_clicked_mouse_pos = t_mouse_pos;
    }

    void draw();

private:
    enum class Mode
    {
        Disabled = 0,
        Referee,
        Robot,
    };

    std::optional<Protos::Ssl::Gc::Referee_Command> refereeCommand() const;
    void refereeUpdate();

    void renderController();

    // UDP socket for ref commands
    std::unique_ptr<Common::UdpServer> m_udp;

    Mode m_controller_mode = Mode::Disabled;

    Common::TeamColor m_referee_color;
    uint32_t          m_command_counter = 0;

    Common::Vec2 m_clicked_mouse_pos = {0., 0.};

    RenderTexture m_render_target;
    Texture2D     m_xbox_texture;
    Texture2D     m_ps5_texture;
};

} // namespace Tyr::Gui
