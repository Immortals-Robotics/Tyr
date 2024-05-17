#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::waitForOmghi(int robot_num, bool chip)
{
    Common::Line ball_line =
        Common::Line::fromPointAndAngle(m_world_state.ball.position, m_world_state.ball.velocity.toAngle());
    if (m_chip_head.deg() < 180)
    {
        ball_line = Common::Line::fromPointAndAngle(m_world_state.ball.position, m_chip_head);
        Common::logDebug("calcing with static head: {}", m_chip_head);
    }
    Common::Line to_goal_line = Common::Line::fromTwoPoints(m_own_robot[robot_num].state().position, oppGoal());

    Common::Vec2 ans = ball_line.intersect(to_goal_line).value_or(Common::Vec2());

    float sBAR;
    sBAR = ans.distanceTo(m_world_state.ball.position);
    sBAR /= m_world_state.ball.velocity.length();
    sBAR = ans.distanceTo(m_own_robot[robot_num].state().position) / sBAR;
    sBAR /= 63.0;
    // sBAR /= 10.0;
    // sBAR /= 1500000;

    Common::logDebug("old sBAR:    {}", sBAR);
    if (sBAR < 5)
        sBAR = 5;
    if (sBAR > 70)
        sBAR = 70;

    Common::Vec2 target = ans; // calculatePassPos(robot_num, 89);

    m_own_robot[robot_num].target.angle = calculateOneTouchAngle(robot_num, target);
    m_own_robot[robot_num].face(Common::Vec2(oppGoal().x, -Common::sign(m_own_robot[robot_num].state().position.y) * 300));

    target = calculatePassPos(robot_num, oppGoal(), m_own_robot[robot_num].state().position, -200);

    Common::logDebug("sBAR:    {}", sBAR);
    VelocityProfile profile = VelocityProfile::kharaki();
    profile.max_spd         = sBAR * 45.0f;

    navigate(robot_num, target, profile);

    m_own_robot[robot_num].shoot(100);
    m_own_robot[robot_num].dribble(15);
}
} // namespace Tyr::Soccer
