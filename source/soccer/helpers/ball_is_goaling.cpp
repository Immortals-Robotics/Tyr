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

    Common::Line ball_line = Common::Line::fromPointAndAngle(ball.position, ball.velocity.toAngle());
    Common::Line targetLine =
        Common::Line::fromTwoPoints(Common::Vec2(side * field_width, -100), Common::Vec2(side * field_width, 100));
    Common::Vec2 ballInter = ball_line.intersect(targetLine).value_or(Common::Vec2());

    Common::debug().drawPoint(ballInter);

    if (std::fabs(ballInter.y) < (goal_width / 2) + 100.0f)
        return true;

    return false;
}
} // namespace Tyr::Soccer
