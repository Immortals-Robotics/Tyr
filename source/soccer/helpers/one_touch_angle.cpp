#include "../ai.h"

namespace Tyr::Soccer
{
Common::Angle Ai::calculateOneTouchAngle(int robot_num, Common::Vec2 oneTouchPosition)
{
    // oneTouchPosition = ball.Position;
    float v0x, v0y;
    float v1x, v1y;

    v0x = ball.velocity.x;
    v0y = ball.velocity.y;

    float goalx = -side * field_width;
    float e;

    float goaly = 0;

    Line targetLine =
        Line::makeLineFromTwoPoints(VecPosition(-side * field_width, -100), VecPosition(-side * field_width, 100));
    Common::Vec2 boz = calculateOpenAngleToGoal(oneTouchPosition, robot_num);
    // boz.x = AngleWith(oneTouchPosition, Common::Vec2(-side*field_width, 0));
    std::cout << "	Open angle : " << boz.x << "	" << boz.y << std::endl;
    Line ball_line = Line::makeLineFromPositionAndAngle(VecPosition(oneTouchPosition.x, oneTouchPosition.y), boz.x);

    VecPosition ans;

    ans = Line::makeLineFromPositionAndAngle(VecPosition(oneTouchPosition.x, oneTouchPosition.y), boz.x + boz.y / 2.0f)
              .getIntersection(targetLine);

    ans = Line::makeLineFromPositionAndAngle(VecPosition(oneTouchPosition.x, oneTouchPosition.y), boz.x - boz.y / 2.0f)
              .getIntersection(targetLine);

    ans = ball_line.getIntersection(targetLine);

    goalx = ans.getX();
    goaly = ans.getY();

    float aa  = -Common::sign(goalx) * 90;
    float max = 6430000;
    float gg  = 0;

    while (aa < 180 - 90 * Common::sign(goalx))
    {
        float a = (aa / 180.0) * 3.14;
        v1x     = beta * (-sin(a) * v0x + cos(a) * v0y) * (-sin(a)) + shootK * cos(a) +
              gamma * (v0x - 2 * (v0x * cos(a) + v0y * sin(a)) * cos(a));
        v1y = beta * (-sin(a) * v0x + cos(a) * v0y) * (cos(a)) + shootK * sin(a) +
              gamma * (v0y - 2 * (v0x * cos(a) + v0y * sin(a)) * sin(a));
        e = v1x * (-OwnRobot[robot_num].State.Position.y + goaly) +
            v1y * (OwnRobot[robot_num].State.Position.x - goalx);
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
