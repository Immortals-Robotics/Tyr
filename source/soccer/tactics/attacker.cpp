#include "attacker.h"

#include "../helpers/ball_prediction.h"
#include "../robot/robot.h"

namespace Tyr::Soccer
{
const Tactic::Id AttackerTactic::kId = &AttackerTactic::kId;

void AttackerTactic::execute(Robot &t_robot)
{
#if 0
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
#else
    oldAttacker(t_robot);
#endif
}

// TODO: this is called in a hot loop, make it inline
float AttackerTactic::calculateRobotReachTime(const Robot &         t_robot, const Common::Vec2 t_dest,
                                              const VelocityProfile t_profile) const
{
    const Trajectory2D trajectory =
        Trajectory2D::makeBangBangTrajectory(t_robot.state().position, t_robot.currentMotion(), t_dest, t_profile);
    return trajectory.getDuration();
}

float AttackerTactic::calculateBallRobotReachTime(const Robot &         t_robot, const Common::Angle angle,
                                                  const VelocityProfile t_profile, const float       t_wait) const
{
    const float t_max          = 5.0;
    float       t_interception = t_max;
    for (float t = 0; t < t_max; t += 0.1f)
    {
        const Common::BallState ball_state = predictBall(t);

        const Common::Vec2 interception_point =
            ball_state.position.circleAroundPoint(angle, Common::field().robot_radius);

        float t_reach = calculateRobotReachTime(t_robot, interception_point, t_profile);
        if (t_reach + t_wait <= t)
        {
            t_interception = t_reach;
            break;
        }
    }
    return t_interception;
}

NavigationFlags AttackerTactic::getNavigationFlags(const Robot &t_robot) const
{
    NavigationFlags navigation_flags = NavigationFlags::None;

    if (State::ref().running() && t_robot.state().position.distanceTo(State::world().ball.position) < 500.0f)
    {
        navigation_flags |= NavigationFlags::ForceNoExtraMargin;
        navigation_flags |= NavigationFlags::ForceNoBreak;
    }

    if (m_is_gk)
    {
        navigation_flags |= NavigationFlags::ForceNoOwnPenaltyArea;
    }

    return navigation_flags;
}

void AttackerTactic::oldAttacker(Robot &t_robot)
{
    const NavigationFlags navigation_flags = getNavigationFlags(t_robot);

    Common::debug().draw(
        Common::LineSegment{State::world().ball.position, State::world().ball.position + m_angle.toUnitVec() * 1000.0f},
        Common::Color::blue());

    const float        t_interception = calculateBallRobotReachTime(t_robot, m_angle, VelocityProfile::mamooli(), 0.1f);
    const Common::Vec2 predicted_ball = predictBall(t_interception).position;

    Common::debug().draw(Common::Circle{predicted_ball, 50}, Common::Color::blue(), false);

    static bool passedBall = false;

    Common::Vec2 ballToGoal = Field::oppGoal() - State::world().ball.position;
    ballToGoal              = ballToGoal.normalized();
    float ballVelToGoalDot  =
        (State::world().ball.velocity.x * ballToGoal.x + State::world().ball.velocity.y * ballToGoal.y);
    Common::Vec2 ballVelToGoal     = ballToGoal * ballVelToGoalDot;
    Common::Vec2 ballVelPrepToGoal = State::world().ball.velocity - ballVelToGoal;

    Common::debug().draw(
        Common::LineSegment{State::world().ball.position, State::world().ball.position + State::world().ball.velocity});
    Common::debug().draw(
        Common::LineSegment{State::world().ball.position, State::world().ball.position + ballVelToGoal},
        Common::Color::pink());
    Common::debug().draw(
        Common::LineSegment{State::world().ball.position, State::world().ball.position + ballVelPrepToGoal},
        Common::Color::green());

    Common::Vec2 robotToBall    = t_robot.state().position - State::world().ball.position;
    float        robotToBallDot = robotToBall.x * ballVelPrepToGoal.x + robotToBall.y * ballVelPrepToGoal.y;
    robotToBallDot /= robotToBall.length();
    robotToBallDot /= ballVelPrepToGoal.length();
    if (robotToBallDot > 0.3)
        passedBall = true;
    else if ((robotToBallDot < -0.2) || (State::world().ball.position.distanceTo(t_robot.state().position) > 1050))
        passedBall = false;

    if (passedBall)
    {
        Common::debug().draw(Common::Circle{t_robot.state().position, 100}, Common::Color::gold(), false);
    }

    if (State::world().ball.position.distanceTo(t_robot.state().position) < 400)
        m_circle_reached_behind_ball = true;
    else if (State::world().ball.position.distanceTo(t_robot.state().position) > 600)
        m_circle_reached_behind_ball = false;

    Common::logDebug("circle dadam");

    float r = 150.0f;

    if (m_kiss)
    {
        r = 82.0f;
    }

    float tetta = 10.0f;

    if ((m_kick > 0) || (m_chip > 0))
    {
        if (State::ref().restart())
        {
            r     = 200.0f;
            tetta = 20.0f;
        }
        else
        {
            r     = 900.0f;
            tetta = 45.0f;
            // if ( !passedBall )
            //	r *= 1.5;
        }
    }

    if (m_dribbler)
    {
        if (t_robot.state().position.distanceTo(State::world().ball.position) < 190)
            t_robot.dribble(15);
    }

    if (State::ref().restart())
        t_robot.face(predicted_ball);
    else
        t_robot.target.angle = m_angle - Common::Angle::fromDeg(180.0f);

    // move the ball forward a bit to compensate for the over predict we do on our robots
    const Common::Vec2 ball_moved_towards_target = predicted_ball - m_angle.toUnitVec() * 100.0f;
    Common::Angle      hehe                      = ball_moved_towards_target.angleWith(t_robot.state().position);
    hehe                                         = m_angle - hehe;

    if ((std::fabs(hehe.deg()) < tetta)) //|| ( m_circle_reached_behind_ball ) )
    {
        Common::debug().draw(Common::Circle{t_robot.state().position, 100}, Common::Color::red(), false);
        if ((m_kick > 0.f) || (m_chip))
        {
            if (m_circle_reached_behind_ball)
            {
                Common::logDebug("reached");
                Common::Vec2 targetPoint;
                if (!State::ref().restart())
                {
                    float normalized_hehe = std::fabs(hehe.deg()) / tetta;
                    normalized_hehe = std::pow(normalized_hehe, 0.3f);
                    targetPoint = predicted_ball.circleAroundPoint(m_angle, std::min(r, normalized_hehe * 265.0f));

                    if (!m_precise)
                    {
                        targetPoint -= m_angle.toUnitVec() * (1.0f - std::pow(normalized_hehe, 0.5f)) * 400.0f;
                    }
                }
                else
                {
                    targetPoint = predicted_ball.circleAroundPoint(
                        m_angle, std::min(r, std::fabs(hehe.deg()) * 320.0f / (tetta)));
                }

                // This part extends the target point towards the goal
                if (m_precise)
                {
                    Common::debug().draw(targetPoint, Common::Color::maroon());

                    Common::logDebug("elendil: {}", m_elendil);
                    Common::Angle hehe2 = predicted_ball.angleWith(t_robot.state().position);
                    hehe2               = m_angle - hehe2;
                    Common::logDebug("hehe2: {}", hehe2.deg());

                    const bool el_in = ((std::abs(hehe2.deg()) < 5.0f) &&
                                        (State::world().ball.position.distanceTo(t_robot.state().position) < 100));
                    const bool el_out = ((std::abs(hehe2.deg()) > 10.0f) &&
                                         (State::world().ball.position.distanceTo(t_robot.state().position) > 200));

                    if (el_in)
                        m_elendil = 30;
                    if (el_out)
                        m_elendil = 0;

                    if (m_elendil > 0)
                    {
                        m_elendil--;
                        // extend towards the shoot target
                        targetPoint -= m_angle.toUnitVec() * 200.0f;
                    }
                    else
                    {
                        // extend towards the point behind the ball
                        // targetPoint += (targetPoint - t_robot.state().position) / 2.0f;
                    }
                }

                t_robot.navigate(targetPoint, VelocityProfile::kharaki(), navigation_flags);
            }
            else
                t_robot.navigate(
                    predicted_ball.circleAroundPoint(m_angle, std::min(r, std::fabs(hehe.deg()) * 320.0f / tetta)),
                    VelocityProfile::mamooli(), navigation_flags);
        }
        else
            t_robot.navigate(predicted_ball.circleAroundPoint(m_angle + hehe * 0.0f, r), VelocityProfile::mamooli(),
                             navigation_flags);
    }
    else
    {
        Common::debug().draw(Common::Circle{t_robot.state().position, 100}, Common::Color::orange(), false);

        hehe = predicted_ball.angleWith(t_robot.state().position) +
               Common::Angle::fromDeg(Common::sign(hehe.deg()) * tetta);
        if (passedBall)
            t_robot.navigate(predicted_ball.circleAroundPoint(hehe, r), VelocityProfile::mamooli(),
                             navigation_flags);
        else
            t_robot.navigate(predicted_ball.circleAroundPoint(hehe, r), VelocityProfile::kharaki(),
                             navigation_flags);
    }

    Common::debug().draw(Common::Circle{t_robot.state().position, 100}, Common::Color::violet(), false);
    Common::debug().draw(
        Common::LineSegment{t_robot.state().position,
                            t_robot.state().position +
                            (t_robot.target.position - t_robot.state().position).normalized() * 1000.0f},
        Common::Color::black());

    if ((m_kick > 0.f) || (m_chip > 0))
    {
        // if ( m_chip )
        t_robot.chip(m_chip);
        // else
        float vel_delta = t_robot.state().velocity.length(); // convert to m/s
        vel_delta       = std::min(4000.0f, vel_delta);
        vel_delta       = m_kick - vel_delta;
        t_robot.shoot(m_kick);
    }
}
} // namespace Tyr::Soccer
