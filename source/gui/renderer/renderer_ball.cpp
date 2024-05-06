#include "renderer.h"
namespace Tyr::Gui
{

void Renderer::drawBalls(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionBall> &data,
                         bool                                                                 draw_goal_lines)
{
    for (const auto &ball : data)
    {
        drawBall(ball, draw_goal_lines);
    }
}

void Renderer::drawBall(const Protos::SSL_DetectionBall &ball, bool draw_goal_lines)
{
    auto position = Common::Vec2(ball.x(), ball.y());
    draw(Common::Circle{position, ballRadius}, Common::Color::orange(), true);
    draw(Common::Circle{position, ballRadius + 10.0f}, Common::Color::black(), false, 1.0f);
    if (draw_goal_lines)
    {
        Common::Vec2 ourgoal_p1 =
            Common::Vec2(Common::worldState().field.width, Common::worldState().field.goal_width / 2);
        Common::Vec2 ourgoal_p2 =
            Common::Vec2(Common::worldState().field.width, Common::worldState().field.goal_width / -2);
        Common::Vec2 oppgoal_p1 = ourgoal_p1 * -1;
        Common::Vec2 oppgoal_p2 = ourgoal_p2 * -1;
        draw(Common::Triangle{ourgoal_p1, position, ourgoal_p2}, Common::Color::blue().transparent(), true);
        draw(Common::Triangle{oppgoal_p1, position, oppgoal_p2}, Common::Color::red().transparent(), true);
    }
}

} // namespace Tyr::Gui
