#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::ballIsGoaling()
{
    bool towardGoal = false;

    if ((side == -1) && (std::fabs(m_world_state.ball.velocity.toAngle().deg()) > 90.0f))
        towardGoal = true;
    if ((side == 1) && (std::fabs(m_world_state.ball.velocity.toAngle().deg()) < 90.0f))
        towardGoal = true;

    if (!towardGoal)
        return false;

    if (m_world_state.ball.velocity.length() < 200.0f)
        return false;

    Common::Line        ball_line  = m_world_state.ball.line();
    Common::LineSegment targetLine = ownGoalLine();

    const auto ballInter = ball_line.intersect(targetLine);

    if (ballInter.has_value())
    {
        Common::debug().draw(ballInter.value());
        return true;
    }

    return false;
}
} // namespace Tyr::Soccer
