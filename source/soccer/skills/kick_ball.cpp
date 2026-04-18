#include <algorithm>

#include "kick_ball.h"

#include "../robot/robot.h"

#include "../helpers/ball_prediction.h"

namespace Tyr::Soccer
{
const Skill::Id KickBallSkill::kId = &KickBallSkill::kId;

void KickBallSkill::execute(Robot &t_robot)
{
    const float kMaxForwardOffset = m_is_gk ? 200.f: 300.0f;
    const auto      smoothstep01      = [](const float t_value)
    {
        const float t = std::clamp(t_value, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    };

    const NavigationFlags navigation_flags = getNavigationFlags(t_robot);

#if 1
    // const float t_interception = calculateBallRobotReachTime(t_robot, -m_angle, VelocityProfile::mamooli(), 0.1f);
    const Common::BallState ball = predictBall(0.2);
    Common::debug().draw(Common::Circle{ball.position, 50}, Common::Color::blue(), false);
#else
    const Common::BallState& ball = State::world().ball;
#endif

    Robot &robot = t_robot;

    // Desired ball travel direction. The robot should approach from the opposite side of this vector.
    const Common::Vec2 to_target = -m_angle.toUnitVec();
    Common::debug().draw(Common::LineSegment{ball.position, ball.position + to_target * 300.0f}, Common::Color::red(),
                         80.0f);
    const Common::Vec2 robot_to_ball = ball.position - robot.state().position;
    const Common::Vec2 to_ball       = robot_to_ball.normalized();

    // +1 means the robot is nicely behind the ball, 0 means mostly to the side, -1 means in front of it.
    const float ball_target_dot = to_target.dot(to_ball);
    // Exponent < 1 makes the clearance grow a bit faster as alignment gets worse.
    const float r_scale_factor  = 2.0f * std::pow((1.0f - ball_target_dot) / 2.f, 0.5f) - 1.0f;

    // Wider path when the robot is in front of / beside the ball, tighter path when already behind it.
    const float r = std::min(m_is_gk ? 70.f + 120.0f * r_scale_factor: 50.0f + 120.0f * r_scale_factor, 500.0f);
    const float behind_ball_distance = Common::field().robot_radius + r;

    const Common::Vec2 behind_ball_pos = ball.position.circleAroundPoint(m_angle, behind_ball_distance);
    const float        alignment_factor = std::clamp(ball_target_dot, 0.0f, 1.0f);// smoothstep01(std::clamp(ball_target_dot, 0.0f, 1.0f));
    const float        axial_distance_to_ball = to_target.dot(robot_to_ball);
    // Once already behind the ball, allow the robot to creep forward toward the kick point.
    const float        distance_progress =
        1.0f - std::clamp(axial_distance_to_ball / behind_ball_distance, 0.0f, 1.0f);
    const float distance_factor = smoothstep01(distance_progress);
    const float forward_offset = kMaxForwardOffset * alignment_factor * distance_factor;
    const Common::Vec2 final_pos = behind_ball_pos + to_target * forward_offset;

    // Match the navigation ball obstacle to the same clearance used for the behind-ball target.
    robot.dynamic_ball_obs_r = r;
    auto vel_profile        = VelocityProfile::mamooli();
    if (t_robot.state().position.distanceTo(ball.position) < 30.0f) {
        vel_profile = VelocityProfile::kharaki();
    }
    t_robot.navigate(final_pos, vel_profile, navigation_flags);

#if 0
    robot.target.angle = t_angle + Common::Angle::fromDeg(180.0f);
#else
    if (m_is_gk) {
        robot.face(ball.position);
    }
    else {
        robot.target.angle = (ball.position - behind_ball_pos).normalized().toAngle();
    }
#endif

    robot.shoot(m_kick);
    robot.chip(m_chip);

    if (!m_is_gk && t_robot.state().position.distanceTo(ball.position) < 200.0f)
    {
        robot.dribble(3.0f);
    }
}

NavigationFlags KickBallSkill::getNavigationFlags(const Robot &t_robot) const
{
    NavigationFlags navigation_flags = NavigationFlags::None;

    navigation_flags |= NavigationFlags::DynamicBallObstacle;

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
