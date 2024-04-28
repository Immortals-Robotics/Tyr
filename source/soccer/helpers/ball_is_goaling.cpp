#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::ballIsGoaling()
{
    bool towardGoal = false;

    if ((side == -1) && (std::fabs(ball.velocity.toAngle().deg()) > 90.0f))
        towardGoal = true;
    if ((side == 1) && (std::fabs(ball.velocity.toAngle().deg()) < 90.0f))
        towardGoal = true;

    if (!towardGoal)
        return false;

    if (ball.velocity.length() < 200.0f)
        return false;

    Line ball_line =
        Line::makeLineFromPositionAndAngle(VecPosition(ball.Position.x, ball.Position.y), ball.velocity.direction);
    Line targetLine =
        Line::makeLineFromTwoPoints(VecPosition(side * field_width, -100), VecPosition(side * field_width, 100));
    VecPosition ballInter = ball_line.getIntersection(targetLine);

    Common::debug().drawPoint(Common::Vec2(ballInter.getX(), ballInter.getY()));

    if (std::fabs(ballInter.getY()) < (goal_width / 2) + 100.0)
        return true;

    return false;
}
} // namespace Tyr::Soccer
