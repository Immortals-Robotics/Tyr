#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::ballIsGoaling()
{
    bool towardGoal = false;

    if ((side == -1) && (std::fabs(m_state.ball.velocity.toAngle().deg()) > 90.0f))
        towardGoal = true;
    if ((side == 1) && (std::fabs(m_state.ball.velocity.toAngle().deg()) < 90.0f))
        towardGoal = true;

    if (!towardGoal)
        return false;

    if (m_state.ball.velocity.length() < 200.0f)
        return false;

    Common::Line ball_line = Common::Line::fromPointAndAngle(m_state.ball.position, m_state.ball.velocity.toAngle());
    Common::Line targetLine =
        Common::Line::fromTwoPoints(Common::Vec2(side * Common::field().width, -100), Common::Vec2(side * Common::field().width, 100));
    Common::Vec2 ballInter = ball_line.intersect(targetLine).value_or(Common::Vec2());

    Common::debug().draw(ballInter);

    if (std::fabs(ballInter.y) < (Common::field().goal_width / 2) + 100.0f)
        return true;

    return false;
}
} // namespace Tyr::Soccer
