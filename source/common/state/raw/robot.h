#pragma once

#include "frame.h"

namespace Immortals::Common
{
struct RawRobotState
{
    unsigned frame_idx;
    RawFrame frame;

    float confidence = 0.0f;

    int       id;
    TeamColor color;

    float height = 0.0f;

    Vec2 position;
    Vec2 pixel_position;

    Angle angle;

    RawRobotState() = default;

    RawRobotState(const Protos::Ssl::Vision::DetectionRobot &t_robot, const TeamColor t_color,
                  const std::vector<RawFrame> &t_frames, const unsigned t_frame_idx)
    {
        frame_idx = t_frame_idx;
        frame     = t_frames[frame_idx];

        confidence = t_robot.confidence();

        id    = t_robot.robot_id();
        color = t_color;

        height = t_robot.height();

        position       = Vec2(t_robot.x(), t_robot.y());
        pixel_position = Vec2(t_robot.pixel_x(), t_robot.pixel_y());

        angle = Angle::fromRad(t_robot.orientation());
    }

    explicit RawRobotState(const Protos::Immortals::RawRobotState &t_robot, const std::vector<RawFrame> &t_frames)
    {
        frame_idx = t_robot.frame_idx();
        frame     = t_frames[frame_idx];

        confidence = t_robot.confidence();

        id    = t_robot.id();
        color = static_cast<TeamColor>(t_robot.color());

        height = t_robot.height();

        position       = Vec2{t_robot.position()};
        pixel_position = Vec2{t_robot.pixel_position()};

        angle = Angle{t_robot.angle()};
    }

    void fillProto(Protos::Immortals::RawRobotState *const t_state) const
    {
        t_state->set_frame_idx(frame_idx);

        t_state->set_confidence(confidence);

        t_state->set_id(id);
        t_state->set_color(static_cast<Protos::Immortals::TeamColor>(color));

        t_state->set_height(height);

        position.fillProto(t_state->mutable_position());
        pixel_position.fillProto(t_state->mutable_pixel_position());

        angle.fillProto(t_state->mutable_angle());
    }
};
} // namespace Immortals::Common
