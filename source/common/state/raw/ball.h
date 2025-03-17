#pragma once

#include "frame.h"

namespace Immortals::Common
{
struct RawBallState
{
    unsigned frame_idx;
    RawFrame frame;

    float confidence = 0.0f;

    Vec3 position;
    Vec2 pixel_position;

    unsigned area = 0;

    RawBallState() = default;

    explicit RawBallState(const Protos::Ssl::Vision::DetectionBall &t_ball, const std::vector<RawFrame> &t_frames,
                          const unsigned t_frame_idx)
    {
        frame_idx = t_frame_idx;
        frame     = t_frames[frame_idx];

        confidence = t_ball.confidence();

        position       = Vec3(t_ball.x(), t_ball.y(), t_ball.z());
        pixel_position = Vec2(t_ball.pixel_x(), t_ball.pixel_y());

        area = t_ball.area();
    }

    explicit RawBallState(const Protos::Immortals::RawBallState &t_ball, const std::vector<RawFrame> &t_frames)
    {
        frame_idx = t_ball.frame_idx();
        frame     = t_frames[frame_idx];

        confidence = t_ball.confidence();

        position       = Vec3{t_ball.position()};
        pixel_position = Vec2{t_ball.pixel_position()};

        area = t_ball.area();
    }

    void fillProto(Protos::Immortals::RawBallState *const t_ball) const
    {
        t_ball->set_frame_idx(frame_idx);

        t_ball->set_confidence(confidence);

        position.fillProto(t_ball->mutable_position());
        pixel_position.fillProto(t_ball->mutable_pixel_position());

        t_ball->set_area(area);
    }
};
} // namespace Immortals::Common
