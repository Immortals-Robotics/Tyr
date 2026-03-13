#pragma once

namespace Tyr::Soccer
{
inline bool ballIsGoaling()
{
    if (State::world().ball.velocity.length() < 200.0f)
        return false;

    bool towardGoal = false;

    if ((State::side() == -1) && (std::fabs(State::world().ball.velocity.toAngle().deg()) > 90.0f))
        towardGoal = true;
    if ((State::side() == 1) && (std::fabs(State::world().ball.velocity.toAngle().deg()) < 90.0f))
        towardGoal = true;

    if (!towardGoal)
        return false;

    const Common::Line        ball_line  = State::world().ball.line();
    const Common::LineSegment goal_line {
        Field::ownGoalPostBottom() - Common::Vec2{0.f, 100.f},
        Field::ownGoalPostTop() + Common::Vec2{0.f, 100.f}};

    const auto ballInter = ball_line.intersect(goal_line);

    if (ballInter.has_value())
    {
        Common::debug().draw(ballInter.value());
        return true;
    }

    return false;
}
} // namespace Tyr::Soccer
