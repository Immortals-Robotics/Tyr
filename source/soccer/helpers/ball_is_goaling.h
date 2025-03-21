#pragma once

namespace Tyr::Soccer
{
inline bool ballIsGoaling()
{
    bool towardGoal = false;

    if ((State::side() == -1) && (std::fabs(State::world().ball.velocity.toAngle().deg()) > 90.0f))
        towardGoal = true;
    if ((State::side() == 1) && (std::fabs(State::world().ball.velocity.toAngle().deg()) < 90.0f))
        towardGoal = true;

    if (!towardGoal)
        return false;

    if (State::world().ball.velocity.length() < 200.0f)
        return false;

    Common::Line        ball_line  = State::world().ball.line();
    Common::LineSegment targetLine = Field::ownGoalLine();

    const auto ballInter = ball_line.intersect(targetLine);

    if (ballInter.has_value())
    {
        Common::debug().draw(ballInter.value());
        return true;
    }

    return false;
}
} // namespace Tyr::Soccer
