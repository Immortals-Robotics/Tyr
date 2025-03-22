#include "attacker.h"

#include "../helpers/ball_prediction.h"
#include "../robot/robot.h"

#define OLD_ATTACKER 1

#if OLD_ATTACKER
#include "../skills/old_attacker.h"
#endif

namespace Tyr::Soccer
{
const Tactic::Id AttackerTactic::kId = &AttackerTactic::kId;

void AttackerTactic::execute(Robot &t_robot)
{
#if OLD_ATTACKER
    OldAttackerSkill{m_angle, m_kick, m_chip, m_kiss, m_dribbler, m_precise, m_is_gk}.execute(t_robot);
#else
    const Common::BallState &ball = State::world().ball;

    if (ball.velocity.length() < 100.0f)
    {
        t_robot.halt();
        return;
    }

    const VelocityProfile profile = VelocityProfile::mamooli();

    Common::debug().draw(
        Common::LineSegment{State::world().ball.position, State::world().ball.position + m_angle.toUnitVec() * 1000.0f},
        Common::Color::blue());

    const Common::Angle interception_angle = Common::Angle::fromVec(ball.velocity);
    const float         t_interception     = calculateBallRobotReachTime(t_robot, interception_angle, profile, 0.1f);

    const Common::BallState ball_state = predictBall(t_interception);
    const Common::Vec2      interception_point =
        ball_state.position.circleAroundPoint(Common::Angle::fromVec(ball.velocity), Common::field().robot_radius);

    Common::debug().draw(interception_point);

    t_robot.navigate(interception_point, profile);
    t_robot.target.angle = Common::Angle::fromVec(ball_state.position - interception_point);
#endif
}

NavigationFlags AttackerTactic::getNavigationFlags(const Robot &t_robot) const
{
    NavigationFlags navigation_flags = NavigationFlags::None;

    if (State::ref().running() && t_robot.state().position.distanceTo(State::world().ball.position) < 500.0f)
    {
        navigation_flags |= NavigationFlags::NoExtraMargin;
        navigation_flags |= NavigationFlags::NoBreak;
    }

    if (m_is_gk)
    {
        navigation_flags |= NavigationFlags::NoOwnPenaltyArea;
    }

    return navigation_flags;
}
} // namespace Tyr::Soccer
