#include "renderer.h"
namespace Tyr::Gui
{
void Renderer::draw(const Common::RawBallState &t_ball)
{
    draw(Common::Circle{t_ball.position.xy(), Common::field().ball_radius}, Common::Color::orange(), true);
    draw(Common::Circle{t_ball.position.xy(), Common::field().ball_radius + 10.0f}, Common::Color::black(), false,
         1.0f);
}

void Renderer::draw(const Common::BallState &t_ball)
{
    if (t_ball.seen_state == Common::SeenState::CompletelyOut)
        return;

    auto outline_color = Common::Color::black();
    auto base_color    = Common::Color::orange();

    if (t_ball.seen_state == Common::SeenState::TemporarilyOut)
    {
        base_color    = base_color.transparent();
        outline_color = outline_color.transparent();
    }

    draw(Common::Circle{t_ball.position, Common::field().ball_radius}, base_color, true);
    draw(Common::Circle{t_ball.position, Common::field().ball_radius + 10.0f}, outline_color, false, 0.5f);

    draw(Common::LineSegment{t_ball.position, t_ball.position + t_ball.velocity}, outline_color, 1.0f);
}

} // namespace Tyr::Gui
