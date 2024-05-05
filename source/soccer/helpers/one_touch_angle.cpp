#include "../ai.h"

namespace Tyr::Soccer
{
Common::Angle Ai::calculateOneTouchAngle(int robot_num, Common::Vec2 oneTouchPosition)
{
    // oneTouchPosition = Common::worldState().ball.position;
    float v0x, v0y;
    float v1x, v1y;

    v0x = Common::worldState().ball.velocity.x;
    v0y = Common::worldState().ball.velocity.y;

    float goalx = -side * Common::worldState().field.width;
    float e;

    float goaly = 0;

    Common::Line targetLine = Common::Line::fromTwoPoints(Common::Vec2(-side * Common::worldState().field.width, -100),
                                                          Common::Vec2(-side * Common::worldState().field.width, 100));

    OpenAngle boz = calculateOpenAngleToGoal(oneTouchPosition, robot_num);
    Common::logDebug("Open angle : {}    {}", boz.center.deg(), boz.magnitude.deg());

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
        v1x     = Common::setting().one_touch_beta * (-sin(a) * v0x + cos(a) * v0y) * (-sin(a)) + Common::setting().one_touch_shoot_k * cos(a) +
              Common::setting().one_touch_gamma * (v0x - 2 * (v0x * cos(a) + v0y * sin(a)) * cos(a));
        v1y = Common::setting().one_touch_beta * (-sin(a) * v0x + cos(a) * v0y) * (cos(a)) + Common::setting().one_touch_shoot_k * sin(a) +
              Common::setting().one_touch_gamma * (v0y - 2 * (v0x * cos(a) + v0y * sin(a)) * sin(a));
        e = v1x * (-OwnRobot[robot_num].state().position.y + goaly) +
            v1y * (OwnRobot[robot_num].state().position.x - goalx);
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
