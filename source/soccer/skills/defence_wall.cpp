#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::DefenceWall(int robot_num, bool kickOff)
{
    float x     = -side * ball.Position.x;
    float tetta = -0.000003f * x * x + 0.0016f * x + 90.0f;
    if (kickOff)
    {
        tetta = 14.0f;
    }

    Common::Vec2 target;

    // std::cout << "	wall limit: " << tetta << std::endl;
    int index = findKickerOpp(-1);
    if (index == -1)
    {
        target = ball.Position.circleAroundPoint(ball.Position.angleWith(Common::Vec2(side * field_width, 0)), 730);
    }
    else
    {
        target = OppRobot[index].Position.pointOnConnectingLine(
            ball.Position, 590 + ball.Position.distanceTo(OppRobot[index].Position));
    }

    // std::cout << index << std::endl;

    Common::Angle ballAngle = ball.Position.angleWith(target);
    Common::Angle firstLeg =
        ball.Position.angleWith(Common::Vec2(side * field_width, Common::sign(ball.Position.y) * (350.0f)));
    Common::Angle secLeg = firstLeg - Common::Angle::fromDeg(tetta * Common::sign(ball.Position.y) * side);

    // std::cout << "	ball: " << ballAngle << "	f: " << firstLeg << "	s: " << secLeg << std::endl;

    bool isOut =
        false; // std::fabs((std::fabs(NormalizeAngle(ballAngle-firstLeg))+std::fabs(NormalizeAngle(ballAngle-secLeg)))
               // - tetta ) > 1.0f;

    if (isOut)
    {
        target = ball.Position.circleAroundPoint(ball.Position.angleWith(Common::Vec2(side * field_width, 0)), 730);
    }

    OwnRobot[robot_num].face(ball.Position);
    ERRTSetObstacles(robot_num, true, true);
    ERRTNavigate2Point(robot_num, target);
}
} // namespace Tyr::Soccer
