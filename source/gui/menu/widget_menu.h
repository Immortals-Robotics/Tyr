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
    /* data */
    void drawTabBar();
    void drawControllerTab();
    int  m_controller_mode = static_cast<int>(ControllerMode::DISABLED);
    RenderTexture m_lstick_texture, m_rstick_texture, m_buttons_texture;

public:
    WidgetMenu();
    ~WidgetMenu() = default;
    void draw();
};

} // namespace Tyr::Gui
