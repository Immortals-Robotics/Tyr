#include "../ai.h"

namespace Tyr::Soccer
{
Common::Angle Ai::calculateOneTouchAngle(const int t_robot_num, const Common::Vec2 t_one_touch_position)
{
    // t_one_touch_position = m_world_state.ball.position;
    float v0x, v0y;
    float v1x, v1y;

    v0x = m_world_state.ball.velocity.x;
    v0y = m_world_state.ball.velocity.y;

    const Common::LineSegment targetLine = oppGoalLine();

    OpenAngle boz = calculateOpenAngleToGoal(t_one_touch_position, t_robot_num);
    Common::logDebug("Open angle : {}    {}", boz.center.deg(), boz.magnitude.deg());

    Common::Line ball_line = Common::Line::fromPointAndAngle(t_one_touch_position, boz.center);

    Common::Vec2 goal = ball_line.intersect(targetLine).value_or(Common::Vec2(oppGoal().x, 0.0f));

    // TODO: cleanup the following

    float aa  = -Common::sign(goal.x) * 90;
    float max = std::numeric_limits<float>::max();
    float gg  = 0;

    while (aa < 180 - 90 * Common::sign(goal.x))
    {
        float a = (aa / 180.0) * 3.14;
        v1x     = Common::setting().soccer.one_touch_beta * (-sin(a) * v0x + cos(a) * v0y) * (-sin(a)) +
              Common::setting().soccer.one_touch_shoot_k * cos(a) +
              Common::setting().soccer.one_touch_gamma * (v0x - 2 * (v0x * cos(a) + v0y * sin(a)) * cos(a));
        v1y = Common::setting().soccer.one_touch_beta * (-sin(a) * v0x + cos(a) * v0y) * (cos(a)) +
              Common::setting().soccer.one_touch_shoot_k * sin(a) +
              Common::setting().soccer.one_touch_gamma * (v0y - 2 * (v0x * cos(a) + v0y * sin(a)) * sin(a));
        float e = v1x * (-m_own_robot[t_robot_num].state().position.y + goal.y) +
                  v1y * (m_own_robot[t_robot_num].state().position.x - goal.x);
        if (std::fabs(e) < max)
        {
            max = std::fabs(e);
            gg  = aa;
        }
        aa++;
    }

    return Common::Angle::fromDeg(gg);
}
} // namespace Tyr::Soccer
