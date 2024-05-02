#include "../ai.h"

namespace Tyr::Soccer
{
Common::Angle Ai::calculateOneTouchAngle(int robot_num, Common::Vec2 oneTouchPosition)
{
    // oneTouchPosition = ball.position;
    float v0x, v0y;
    float v1x, v1y;

    v0x = ball.velocity.x;
    v0y = ball.velocity.y;

    float goalx = -side * field_width;
    float e;

    float goaly = 0;

    Common::Line targetLine =
        Common::Line::fromTwoPoints(Common::Vec2(-side * field_width, -100), Common::Vec2(-side * field_width, 100));
    OpenAngle boz = calculateOpenAngleToGoal(oneTouchPosition, robot_num);
    // boz.x = AngleWith(oneTouchPosition, Common::Vec2(-side*field_width, 0));
    std::cout << "	Open angle : " << boz.center.deg() << "	" << boz.magnitude.deg() << std::endl;
    Common::Line ball_line = Common::Line::fromPointAndAngle(oneTouchPosition, boz.center);

    Common::Vec2 ans;

    ans = Common::Line::fromPointAndAngle(oneTouchPosition, boz.center + boz.magnitude / 2.0f)
              .intersect(targetLine)
              .value_or(Common::Vec2());

    ans = Common::Line::fromPointAndAngle(oneTouchPosition, boz.center - boz.magnitude / 2.0f)
              .intersect(targetLine)
              .value_or(Common::Vec2());

    ans = ball_line.intersect(targetLine).value_or(Common::Vec2());

    goalx = ans.x;
    goaly = ans.y;

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
        e = v1x * (-OwnRobot[robot_num].State.position.y + goaly) +
            v1y * (OwnRobot[robot_num].State.position.x - goalx);
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
