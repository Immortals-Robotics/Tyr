#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::waitForOmghi(Robot &t_robot)
{
    Common::Line ball_line =
        Common::Line::fromPointAndAngle(m_world_state.ball.position, m_world_state.ball.velocity.toAngle());
    if (m_chip_head.deg() < 180)
    {
        ball_line = Common::Line::fromPointAndAngle(m_world_state.ball.position, m_chip_head);
        Common::logDebug("calcing with static head: {}", m_chip_head);
    }
    Common::Line to_goal_line = Common::Line::fromTwoPoints(t_robot.state().position, oppGoal());

    Common::Vec2 ans = ball_line.intersect(to_goal_line).value_or(Common::Vec2());

    float sBAR;
    sBAR = ans.distanceTo(m_world_state.ball.position);
    sBAR /= m_world_state.ball.velocity.length();
    sBAR = ans.distanceTo(t_robot.state().position) / sBAR;
    sBAR /= 63.0;
    // sBAR /= 10.0;
    // sBAR /= 1500000;

    Common::logDebug("old sBAR:    {}", sBAR);
    if (sBAR < 5)
        sBAR = 5;
    if (sBAR > 70)
        sBAR = 70;

    Common::Vec2 target = ans; // calculatePassPos(t_robot_num, 89);

    t_robot.target.angle = calculateOneTouchAngle(t_robot, target);
    t_robot.face(Common::Vec2(oppGoal().x, -Common::sign(t_robot.state().position.y) * 300));

    target = calculatePassPos(oppGoal(), t_robot.state().position, -200);

    Common::logDebug("sBAR:    {}", sBAR);
    VelocityProfile profile = VelocityProfile::kharaki();
    profile.speed           = sBAR * 45.0f;

    t_robot.navigate(target, profile);

    // TODO: What is this number
    t_robot.shoot(6500.f);
    t_robot.dribble(15);
}
} // namespace Tyr::Soccer
