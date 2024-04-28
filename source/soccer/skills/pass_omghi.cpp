#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::WaitForOmghi(int robot_num, bool chip)
{
    Common::Line ball_line = Common::Line::fromPointAndAngle(ball.Position, ball.velocity.toAngle());
    if (chip_head.deg() < 180)
    {
        ball_line = Common::Line::fromPointAndAngle(ball.Position, chip_head);
        std::cout << "	calcing with static head: " << chip_head.deg() << std::endl;
    }
    Common::Line to_goal_line = Common::Line::fromTwoPoints(
        Common::Vec2(OwnRobot[robot_num].State.Position.x, OwnRobot[robot_num].State.Position.y),
        Common::Vec2(-side * field_width, 0));

    Common::Vec2 ans = ball_line.intersect(to_goal_line).value_or(Common::Vec2());

    float sBAR;
    sBAR = ans.distanceTo(ball.Position);
    sBAR /= ball.velocity.length();
    sBAR = ans.distanceTo(OwnRobot[robot_num].State.Position) / sBAR;
    sBAR /= 63.0;
    // sBAR /= 10.0;
    // sBAR /= 1500000;

    std::cout << "old sBAR:	" << sBAR << "	";
    if (sBAR < 5)
        sBAR = 5;
    if (sBAR > 70)
        sBAR = 70;

    Common::Vec2 target = ans; // CalculatePassPos(robot_num, 89);

    OwnRobot[robot_num].target.angle = calculateOneTouchAngle(robot_num, target);
    OwnRobot[robot_num].face(
        Common::Vec2(-side * field_width, -Common::sign(OwnRobot[robot_num].State.Position.y) * 300));

    ERRTSetObstacles(robot_num);

    target =
        CalculatePassPos(robot_num, Common::Vec2(-side * field_width, 0), OwnRobot[robot_num].State.Position, -200);

    std::cout << "sBAR:	" << sBAR << std::endl;
    ERRTNavigate2Point(robot_num, target, 0, sBAR, &VELOCITY_PROFILE_KHARAKI);

    OwnRobot[robot_num].Shoot(100);
    OwnRobot[robot_num].Dribble(15);
}
} // namespace Tyr::Soccer
