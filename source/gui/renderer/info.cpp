#include "renderer.h"
namespace Tyr::Gui
{
void Renderer::draw(const Common::Referee::State &t_referee_state, const Common::FieldState &t_field)
{
    Common::Vec2 top_left_corner =
        Common::Vec2(t_field.width * -1 - 2 * t_field.boundary_width, t_field.height + 2 * t_field.boundary_width);

    draw(top_left_corner + Common::Vec2(t_field.width * 2 + t_field.boundary_width, -150.),
         "GUI FPS: " + std::to_string(GetFPS()), Common::Color::black(), 200);

    const std::string ref_state = fmt::format("Ref: {}", t_referee_state);
    const float       textSize  = MeasureTextEx(m_font, ref_state.c_str(), 400, 0).x;

    draw(top_left_corner + Common::Vec2(150. + textSize / 2, -150.), ref_state, Common::Color::red(), 400);
}

} // namespace Tyr::Gui
