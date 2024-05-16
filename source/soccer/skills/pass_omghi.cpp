#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::WaitForOmghi(int robot_num, bool chip)
{
    Common::Line ball_line = Common::Line::fromPointAndAngle(m_world_state.ball.position,
                                                             m_world_state.ball.velocity.toAngle());
    if (chip_head.deg() < 180)
    {
        ball_line = Common::Line::fromPointAndAngle(m_world_state.ball.position, chip_head);
        Common::logDebug("calcing with static head: {}", chip_head);
    }
    Common::Line to_goal_line = Common::Line::fromTwoPoints(
        Common::Vec2(OwnRobot[robot_num].state().position.x, OwnRobot[robot_num].state().position.y),
        Common::Vec2(-side * Common::field().width, 0));

    Common::Vec2 ans = ball_line.intersect(to_goal_line).value_or(Common::Vec2());

    float sBAR;
    sBAR = ans.distanceTo(m_world_state.ball.position);
    sBAR /= m_world_state.ball.velocity.length();
    sBAR = ans.distanceTo(OwnRobot[robot_num].state().position) / sBAR;
    sBAR /= 63.0;
    // sBAR /= 10.0;
    // sBAR /= 1500000;

    Common::logDebug("old sBAR:    {}", sBAR);
    if (sBAR < 5)
        sBAR = 5;
    if (sBAR > 70)
        sBAR = 70;

    Common::Vec2 target = ans; // CalculatePassPos(robot_num, 89);

    OwnRobot[robot_num].target.angle = calculateOneTouchAngle(robot_num, target);
    OwnRobot[robot_num].face(Common::Vec2(-side * Common::field().width,
                                          -Common::sign(OwnRobot[robot_num].state().position.y) * 300));

    ERRTSetObstacles(robot_num);

    target = CalculatePassPos(robot_num, Common::Vec2(-side * Common::field().width, 0),
                              OwnRobot[robot_num].state().position, -200);

    Common::logDebug("sBAR:    {}", sBAR);
    ERRTNavigate2Point(robot_num, target, sBAR, VelocityProfile::Type::Kharaki);

    OwnRobot[robot_num].shoot(100);
    OwnRobot[robot_num].dribble(15);
}
} // namespace Tyr::Soccer
