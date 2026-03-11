#include "turn_and_shoot.h"

#include "../robot/robot.h"
#include "kick_ball.h"

namespace Tyr::Soccer
{
const Skill::Id TurnAndShootSkill::kId = &TurnAndShootSkill::kId;

void TurnAndShootSkill::execute(Robot &t_robot)
{
    const Common::Vec2  shoot_dir   = -m_angle.toUnitVec();
    const Common::Angle shoot_angle = shoot_dir.toAngle();
    const Common::Vec2  ball_pos    = State::world().ball.position;

    const float        ball_dist    = ball_pos.distanceTo(t_robot.state().position);
    const Common::Vec2 robot_front  = t_robot.state().angle.toUnitVec();

    const Common::Vec2 robot_to_ball_raw = ball_pos - t_robot.state().position;
    const bool ball_behind = ball_dist < Common::field().robot_radius * 1.5f &&
                             robot_front.dot(robot_to_ball_raw) < 0.0f;
    const bool ball_inside = ball_dist < Common::field().robot_radius * 0.8f;

    const Common::Vec2 effective_ball_pos =
        (ball_behind || ball_inside)
            ? t_robot.state().position + robot_front * Common::field().robot_radius
            : ball_pos;

    const Common::Angle to_ball_angle =
        (effective_ball_pos - t_robot.state().position).toAngle();

    const float angle_to_target = (shoot_angle - to_ball_angle).deg();
    const float angle_error     = std::fabs(angle_to_target);

    Common::logDebug("turn_and_shoot angle_error: {}", angle_error);
    Common::logDebug("turn_and_shoot ball_dist: {}", ball_dist);

    // Once nearly aligned, hand off to kick_ball for the final approach and shot
    if (angle_error < 15.0f)
    {
        KickBallSkill{m_angle, m_kick, m_chip}.execute(t_robot);
        return;
    }

    // Not aligned: wrap around the ball to rotate toward shoot direction
    constexpr float max_steering_angle = 15.0f;

    float angle_offset = angle_to_target;
    if (std::fabs(angle_offset) > max_steering_angle)
        angle_offset = (angle_to_target > 0.0f ? 1.0f : -1.0f) * max_steering_angle;

    const Common::Angle heading     = to_ball_angle + Common::Angle::fromDeg(angle_offset);
    const Common::Vec2  heading_dir = heading.toUnitVec();

    NavigationFlags flags = NavigationFlags::NoExtraMargin | NavigationFlags::NoBreak;

    const float        turn_sign   = (angle_to_target > 0.0f) ? 1.0f : -1.0f;
    const Common::Vec2 behind      = -heading_dir * (Common::field().robot_radius + 20.0f);
    const Common::Vec2 side_offset =
        (heading - Common::Angle::fromDeg(turn_sign * 90.0f)).toUnitVec()
        * Common::field().robot_radius * 0.5f;

    t_robot.navigate(effective_ball_pos + behind + side_offset,
                     VelocityProfile::mamooli(), flags);
    t_robot.target.angle = heading;

    Common::debug().draw(
        Common::LineSegment{effective_ball_pos, effective_ball_pos + heading_dir * 300.0f},
        Common::Color::green());
    Common::debug().draw(
        Common::LineSegment{effective_ball_pos, effective_ball_pos + shoot_dir * 300.0f},
        Common::Color::red());
}
} // namespace Tyr::Soccer
