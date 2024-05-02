#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::DefenceWall(int robot_num, bool kickOff)
{
    float x     = -side * ball.position.x;
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
        target = ball.position.circleAroundPoint(ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)), 730);
    }
    else
    {
        target = OppRobot[index].position.pointOnConnectingLine(
            ball.position, 590 + ball.position.distanceTo(OppRobot[index].position));
    }

    // std::cout << index << std::endl;

    Common::Angle ballAngle = ball.position.angleWith(target);
    Common::Angle firstLeg =
        ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, Common::sign(ball.position.y) * (350.0f)));
    Common::Angle secLeg = firstLeg - Common::Angle::fromDeg(tetta * Common::sign(ball.position.y) * side);

    // std::cout << "	ball: " << ballAngle << "	f: " << firstLeg << "	s: " << secLeg << std::endl;

    bool isOut =
        false; // std::fabs((std::fabs(NormalizeAngle(ballAngle-firstLeg))+std::fabs(NormalizeAngle(ballAngle-secLeg)))
               // - tetta ) > 1.0f;

    if (isOut)
    {
        target = ball.position.circleAroundPoint(ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)), 730);
    }

    OwnRobot[robot_num].face(ball.position);
    ERRTSetObstacles(robot_num, true, true);
    ERRTNavigate2Point(robot_num, target);
}
} // namespace Tyr::Soccer
