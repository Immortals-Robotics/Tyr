#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::ballIsGoaling()
{
    bool towardGoal = false;

    if ((side == -1) && (std::fabs(Common::worldState().ball.velocity.toAngle().deg()) > 90.0f))
        towardGoal = true;
    if ((side == 1) && (std::fabs(Common::worldState().ball.velocity.toAngle().deg()) < 90.0f))
        towardGoal = true;

    if (!towardGoal)
        return false;

    if (Common::worldState().ball.velocity.length() < 200.0f)
        return false;

    Common::Line ball_line = Common::Line::fromPointAndAngle(Common::worldState().ball.position, Common::worldState().ball.velocity.toAngle());
    Common::Line targetLine =
        Common::Line::fromTwoPoints(Common::Vec2(side * Common::worldState().field.width, -100), Common::Vec2(side * Common::worldState().field.width, 100));
    Common::Vec2 ballInter = ball_line.intersect(targetLine).value_or(Common::Vec2());

    Common::debug().draw(ballInter);

    if (std::fabs(ballInter.y) < (Common::worldState().field.goal_width / 2) + 100.0f)
        return true;

    return false;
}
} // namespace Tyr::Soccer
