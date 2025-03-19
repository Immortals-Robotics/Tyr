#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::attacker(const int t_robot_num, const Common::Angle t_angle, const float t_kick, const int t_chip,
                  const bool t_kiss, const bool t_dribbler, const bool precise)
{
    (void) t_robot_num;
    (void) t_angle;
    (void) t_kick;
    (void) t_chip;
    (void) t_kiss;
    (void) t_dribbler;
    (void) precise;

    Robot& robot = m_own_robot[t_robot_num];
    const Common::BallState& ball = m_world_state.ball;

    if (ball.velocity.length() < 100.0f)
    {
        robot.halt();
        return;
    }

    const VelocityProfile profile = VelocityProfile::mamooli();

    Common::debug().draw(
        Common::LineSegment{m_world_state.ball.position, m_world_state.ball.position + t_angle.toUnitVec() * 1000.0f},
        Common::Color::blue());

    const Common::Angle interception_angle = Common::Angle::fromVec(ball.velocity);
    const float       t_interception = calculateBallRobotReachTime(t_robot_num, interception_angle, profile, 0.1f);

    const Common::BallState ball_state = predictBall(t_interception);
    const Common::Vec2 interception_point = ball_state.position.circleAroundPoint(
            Common::Angle::fromVec(ball.velocity),
            Common::field().robot_radius);

    Common::debug().draw(interception_point);

    robot.navigate(interception_point, profile);
    robot.target.angle = Common::Angle::fromVec(ball_state.position - interception_point);
}
} // namespace Tyr::Soccer
