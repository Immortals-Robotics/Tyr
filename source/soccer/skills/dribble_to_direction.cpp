#include "dribble_to_direction.h"

#include "../robot/robot.h"

namespace Tyr::Soccer
{
const Skill::Id DribbleToDirectionSkill::kId = &DribbleToDirectionSkill::kId;

void DribbleToDirectionSkill::execute(Robot &t_robot)
{
    const Common::Vec2  target_dir   = -m_direction.toUnitVec();
    const Common::Angle target_angle = target_dir.toAngle();
    const Common::Vec2  ball_pos     = State::world().ball.position;

    const float        ball_dist    = ball_pos.distanceTo(t_robot.state().position);
    const Common::Vec2 robot_front  = t_robot.state().angle.toUnitVec();

    // When pushing fast, latency can cause the ball to be predicted inside or
    // behind the robot. Detect this by checking if the ball is behind the
    // robot's facing direction (angle > 90°) or very close.
    const Common::Vec2 robot_to_ball_raw = ball_pos - t_robot.state().position;
    const bool ball_behind = ball_dist < Common::field().robot_radius * 1.5f &&
                             robot_front.dot(robot_to_ball_raw) < 0.0f;
    const bool ball_inside = ball_dist < Common::field().robot_radius * 0.8f;

    const Common::Vec2 effective_ball_pos =
        (ball_behind || ball_inside)
            ? t_robot.state().position + robot_front * Common::field().robot_radius
            : ball_pos;

    // Direction from robot to ball — always the base for our heading
    const Common::Angle to_ball_angle =
        (effective_ball_pos - t_robot.state().position).toAngle();

    // How far the target direction is from the robot-to-ball direction
    const float angle_to_target = (target_angle - to_ball_angle).deg();

    // Max steering angle: how far from the robot-to-ball direction we steer
    // toward the target. Larger = tighter arc, smaller = gentler.
    // Clamped to the actual remaining angle so we don't overshoot.
    constexpr float max_steering_angle = 15.0f; // degrees — tunable

    float angle_offset = angle_to_target;
    if (std::fabs(angle_offset) > max_steering_angle)
        angle_offset = (angle_to_target > 0.0f ? 1.0f : -1.0f) * max_steering_angle;

    const Common::Angle heading     = to_ball_angle + Common::Angle::fromDeg(angle_offset);
    const Common::Vec2  heading_dir = heading.toUnitVec();

    NavigationFlags flags = NavigationFlags::NoExtraMargin | NavigationFlags::NoBreak;

    // Position the robot behind the ball relative to the heading direction,
    // offset to the opposite side of the turn. This makes the robot wrap
    // around the ball during the turn.
    // E.g. turning left → robot approaches from the right-behind of the ball.
    const float        turn_sign  = (angle_to_target > 0.0f) ? 1.0f : -1.0f;
    const Common::Vec2 behind     = -heading_dir * (Common::field().robot_radius + 20.0f);
    const Common::Vec2 side_offset =
        (heading - Common::Angle::fromDeg(turn_sign * 90.0f)).toUnitVec()
        * Common::field().robot_radius * 0.5f;

    t_robot.navigate(effective_ball_pos + behind + side_offset,
                     VelocityProfile::mamooli(), flags);
    t_robot.target.angle = heading;

    Common::logDebug("dribble_to_dir to_ball_angle: {}", to_ball_angle.deg());
    Common::logDebug("dribble_to_dir angle_to_target: {}", angle_to_target);
    Common::logDebug("dribble_to_dir ball_dist: {}", ball_dist);

    Common::debug().draw(
        Common::LineSegment{effective_ball_pos, effective_ball_pos + heading_dir * 300.0f},
        Common::Color::green());
    Common::debug().draw(
        Common::LineSegment{effective_ball_pos, effective_ball_pos + target_dir * 300.0f},
        Common::Color::red());
}
} // namespace Tyr::Soccer
