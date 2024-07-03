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

    void renderController();

    RenderTexture m_render_target;
    Texture2D     m_xbox_texture;
    Texture2D     m_ps5_texture;

    // Referee
    std::optional<Protos::Ssl::Gc::Referee_Command> refereeCommand() const;
    void                                            refereeUpdate();

    // UDP socket for ref commands
    std::unique_ptr<Common::UdpServer> m_udp;

    Mode m_controller_mode = Mode::Disabled;

    uint32_t m_command_counter = 0;

    Common::TeamColor m_referee_color = Common::TeamColor::Blue;
    Common::TeamSide  m_our_side      = Common::TeamSide::Left;
    unsigned          m_gk_blue       = 0;
    unsigned          m_gk_yellow     = 0;

    Common::Vec2 m_clicked_mouse_pos = {0., 0.};
};

} // namespace Tyr::Gui
