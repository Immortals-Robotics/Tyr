#include "renderer.h"
namespace Tyr::Gui
{
void Renderer::draw(const Common::BallState &t_ball, bool t_draw_goal_lines)
{
    draw(Common::Circle{t_ball.position, Common::field().ball_radius}, Common::Color::orange(), true);
    draw(Common::Circle{t_ball.position, Common::field().ball_radius + 10.0f}, Common::Color::black(), false,
         1.0f);

    // TODO: move this to soccer
    if (t_draw_goal_lines)
    {
        Common::Vec2 ourgoal_p1 =
            Common::Vec2(Common::field().width, Common::field().goal_width / 2);
        Common::Vec2 ourgoal_p2 =
            Common::Vec2(Common::field().width, Common::field().goal_width / -2);
        Common::Vec2 oppgoal_p1 = ourgoal_p1 * -1;
        Common::Vec2 oppgoal_p2 = ourgoal_p2 * -1;
        draw(Common::Triangle{ourgoal_p1, t_ball.position, ourgoal_p2}, Common::Color::blue().transparent(), true);
        draw(Common::Triangle{oppgoal_p1, t_ball.position, oppgoal_p2}, Common::Color::red().transparent(), true);
    }
}

} // namespace Tyr::Gui
