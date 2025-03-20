#include "attacker.h"

#include "../robot/robot.h"
#include "../helpers/ball_prediction.h"

namespace Tyr::Soccer
{
const Tactic::Id AttackerTactic::kId = &AttackerTactic::kId;

void AttackerTactic::execute(Robot &t_robot)
{
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
}

// TODO: this is called in a hot loop, make it inline
float AttackerTactic::calculateRobotReachTime(const Robot& t_robot, const Common::Vec2 t_dest, const VelocityProfile t_profile) const
{
    const Trajectory2D trajectory = Trajectory2D::makeBangBangTrajectory(
        t_robot.state().position, t_robot.currentMotion(), t_dest, t_profile);
    return trajectory.getDuration();
}

float AttackerTactic::calculateBallRobotReachTime(const Robot& t_robot, const Common::Angle angle, const VelocityProfile t_profile, const float t_wait) const
{
    const float t_max  = 5.0;
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
} // namespace Tyr::Soccer
