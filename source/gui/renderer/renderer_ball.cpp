#include "renderer.h"

namespace Tyr::Gui
{

void Renderer::drawBalls(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionBall> &data, bool draw_goal_lines)
{
    for (const auto &ball : data)
    {
        drawBall(ball, draw_goal_lines);
    }
}

void Renderer::drawBall(const Protos::SSL_DetectionBall &ball, bool draw_goal_lines)
{
    auto position = Common::Vec2(ball.x(), ball.y());
    drawCircle({position, ballRadius}, ORANGE, true);
    drawCircle({position, ballRadius + 10.0f}, BLACK, false, 1.0f);
    if (draw_goal_lines)
    {
        Common::Vec2 ourgoal_p1 = Common::Vec2(Common::setting().field_width / -2, Common::setting().goal_length / 2);
        Common::Vec2 ourgoal_p2 = Common::Vec2(Common::setting().field_width / -2, Common::setting().goal_length / -2);
        Common::Vec2 oppgoal_p1 = ourgoal_p1 * -1;
        Common::Vec2 oppgoal_p2 = ourgoal_p2 * -1;
        drawTriangle(ourgoal_p1, position, ourgoal_p2, BLUE, true, 70);
        drawTriangle(oppgoal_p1, position, oppgoal_p2, RED, true, 70);
    }
}

} // namespace Tyr::Gui
