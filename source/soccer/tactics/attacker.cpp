#include "attacker.h"

#include "../robot/robot.h"

#define OLD_ATTACKER 0

#include "../helpers/ball_prediction.h"
#include "../skills/intercept_ball.h"
#include "../skills/one_touch.h"
#include "../skills/wait_for_ball.h"
#include "../skills/old_attacker.h"
#include "../skills/kick_ball.h"

namespace Tyr::Soccer
{
const Tactic::Id AttackerTactic::kId = &AttackerTactic::kId;

void AttackerTactic::execute(Robot &t_robot)
{
    if (State::timer().time().seconds() < 0.2f || last_robot_id != t_robot.state().vision_id)
    {
        m_state = EState::None;
    }

    last_robot_id = t_robot.state().vision_id;

#if OLD_ATTACKER
    OldAttackerSkill{m_angle, m_kick, m_chip}.execute(t_robot);
#else
    const Common::Line ball_line = State::world().ball.line();

    const Common::Vec2 ball_to_goal = -(m_angle.toUnitVec()) * 1000.0f;
    const Common::Vec2 to_ball      = State::world().ball.position - t_robot.state().position;

    const float ball_line_d = ball_line.distanceTo(t_robot.state().position);

    const bool ball_towards_me = std::fabs(State::world().ball.velocity.angleDiff(-to_ball).deg()) < 90;

    const bool ball_rolling_towards_goal = State::world().ball.velocity.length() > 0.0f && std::fabs(
                                               State::world().ball.velocity.angleDiff(ball_to_goal).deg()) <
                                           15.0f;

    Common::debug().draw(Common::LineSegment{State::world().ball.position, State::world().ball.position + ball_to_goal});
    Common::debug().draw(Common::LineSegment{t_robot.state().position, t_robot.state().position + to_ball});

    const bool behind_ball_for_goal = std::fabs(ball_to_goal.angleDiff(to_ball).deg()) < 45.0;

    const bool rolling_kick_feasible = ball_rolling_towards_goal && !ball_towards_me;

    Common::logDebug("ball_line_d: {}", ball_line_d);
    Common::logDebug("ball_towards_me: {}", ball_towards_me);
    Common::logDebug("ball_rolling_towards_goal: {}", ball_rolling_towards_goal);
    Common::logDebug("behind_ball_for_goal: {}", behind_ball_for_goal);
    Common::logDebug("rolling_kick_feasible: {}", rolling_kick_feasible);

    // state transitions
    if (m_state == EState::None)
    {
        const bool ball_rolling = State::world().ball.velocity.length() > 100.0f;

        if (!ball_rolling || rolling_kick_feasible)
        {
            m_state = EState::Kick;
        }
        else if (ball_towards_me && ball_line_d < 200.0f)
        {
            m_state = EState::WaitForBall;
        }
        else
        {
            m_state = EState::Interception;
        }
    }
    else if (m_state == EState::Interception)
    {
        const bool ball_rolling = State::world().ball.velocity.length() > 100.0f;

        if (!ball_rolling)
        {
            m_state = EState::Kick;
        }
        else if (ball_towards_me && ball_line_d < 200.0f)
        {
            m_state = EState::WaitForBall;
        }
    }
    else if (m_state == EState::WaitForBall)
    {
        if (!ball_towards_me || ball_line_d > 1000.0f)
        {
            const bool ball_rolling = State::world().ball.velocity.length() > 100.0f;

            if (!ball_rolling || rolling_kick_feasible)
            {
                m_state = EState::Kick;
            }
            else
            {
                m_state = EState::Interception;
            }
        }
    }
    else if (m_state == EState::Kick)
    {
        // TODO: change these to ball interception t
        const bool ball_rolling = State::world().ball.velocity.length() > 300.0f;
        const bool ball_too_fast = State::world().ball.velocity.length() > 3000.0f;
        const bool ball_too_far = State::world().ball.position.distanceTo(t_robot.state().position) > 150.0f;

        if (ball_too_fast || (ball_rolling && !rolling_kick_feasible && ball_too_far))
        {
            m_state = EState::Interception;
        }
    }

    const char *state_str = "unknown";
    switch (m_state)
    {
    case EState::Interception:
        state_str = "interception";
        break;
    case EState::WaitForBall:
        state_str = "wait_for_ball";
        break;
    case EState::Kick:
        state_str = "kick";
        break;
    case EState::None:
        state_str = "none";
        break;
    }

    Common::logDebug("attacker state: {}", state_str);

    // state execution
    if (m_state == EState::Kick)
    {
        const bool angle_correct = std::fabs((t_robot.state().angle - ball_to_goal.toAngle()).deg()) < 5.0f;

        const float kick = angle_correct ? m_kick : 1.0f;
        const float chip = angle_correct ? m_chip : 0.0f;

        const bool ball_rolling = State::world().ball.velocity.length() > 100.0f;

        if (ball_rolling)
            OldAttackerSkill{m_angle, kick, chip}.execute(t_robot);
        else
            KickBallSkill{m_angle, kick, chip}.execute(t_robot);

    }
    else if (m_state == EState::WaitForBall)
    {
        const Common::Vec2 target_to_goal = m_goal ? Field::oppGoal() - t_robot.target.position : ball_to_goal;
        const bool one_touch_feasible = std::fabs(target_to_goal.angleDiff(-State::world().ball.velocity).deg()) < 60.0;

        if (one_touch_feasible)
        {
            const Common::Vec2 target = t_robot.state().position + ball_to_goal;
            OneTouchSkill{ m_goal ? nullptr : &target, m_kick, m_chip}.execute(t_robot);
        }
        else
        {
            WaitForBallSkill{{}}.execute(t_robot);
        }

    }
    else if (m_state == EState::Interception)
    {
        InterceptBallSkill{m_angle, 1.0f}.execute(t_robot);
    }
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

    return navigation_flags;
}
} // namespace Tyr::Soccer
