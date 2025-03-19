#include "../ai.h"

namespace Tyr::Soccer
{
// TODO: this is a duplicate of vision/filtered/tracked/ball.h
Common::BallState Ai::predictBall(const float t_time_ahead) const
{
    // TODO: move these to the ball system model

    const Common::Vec2 position = m_world_state.ball.position;
    const Common::Vec2 velocity = m_world_state.ball.velocity;

    // can't predict stationary balls
    if (Common::almostEqual(velocity.length(), 0.0f))
    {
        return m_world_state.ball;
    }

    // TODO: add acceleration to the state vector and let kalman compute it
    const float k  = Common::config().vision.ball_rolling_friction; // ball deceleration (mm/s2)
    const Common::Vec2 deceleration = -velocity.normalized() * k;

    const float time_to_stop = velocity.length() / k;

    const float prediction_time = std::min(time_to_stop, t_time_ahead);

    const Common::Vec2 predicted_velocity = velocity + deceleration * prediction_time;
    const Common::Vec2 displacement       = (velocity + predicted_velocity) * (0.5f * prediction_time);

    const Common::Vec2 predicted_position = position + displacement;

    Common::BallState predicted_state = m_world_state.ball;
    predicted_state.position = predicted_position;
    predicted_state.velocity = predicted_velocity;
    return predicted_state;
}

float Ai::calculateRobotReachTime(const int t_robot_num, const Common::Vec2 t_dest, const VelocityProfile t_profile) const
{
    const Robot& robot = m_own_robot[t_robot_num];
    const Trajectory2D trajectory = Trajectory2D::makeBangBangTrajectory(
        robot.state().position, robot.currentMotion(), t_dest, t_profile);
    return trajectory.getDuration();
}

float Ai::calculateBallRobotReachTime(const int t_robot_num, const Common::Angle angle, const VelocityProfile t_profile, const float t_wait) const
{
    const float t_max  = 5.0;
    float       t_interception = t_max;
    for (float t = 0; t < t_max; t += 0.1f)
    {
        const Common::BallState ball_state = predictBall(t);

        const Common::Vec2 interception_point =
            ball_state.position.circleAroundPoint(angle, Common::field().robot_radius);

        float t_reach = calculateRobotReachTime(t_robot_num, interception_point, t_profile);
        if (t_reach + t_wait <= t)
        {
            t_interception = t_reach;
            break;
        }
    }
    return t_interception;
}
} // namespace Tyr::Soccer
