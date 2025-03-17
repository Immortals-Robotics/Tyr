#pragma once

namespace Immortals::Common
{
struct RawFrame
{
    unsigned camera_id    = 0;
    unsigned frame_number = 0;

    TimePoint t_capture;
    TimePoint t_sent;
    TimePoint t_capture_camera;

    RawFrame() = default;

    explicit RawFrame(const Protos::Ssl::Vision::DetectionFrame &t_frame)
    {
        camera_id    = t_frame.camera_id();
        frame_number = t_frame.frame_number();

        t_capture        = TimePoint::fromSeconds(t_frame.t_capture());
        t_sent           = TimePoint::fromSeconds(t_frame.t_sent());
        t_capture_camera = TimePoint::fromSeconds(t_frame.t_capture_camera());
    }

    explicit RawFrame(const Protos::Immortals::RawFrame &t_frame)
    {
        camera_id    = t_frame.camera_id();
        frame_number = t_frame.frame_number();

        t_capture        = TimePoint::fromMicroseconds(t_frame.t_capture());
        t_sent           = TimePoint::fromMicroseconds(t_frame.t_sent());
        t_capture_camera = TimePoint::fromMicroseconds(t_frame.t_capture_camera());
    }

    void fillProto(Protos::Immortals::RawFrame *const t_frame) const
    {
        t_frame->set_camera_id(camera_id);
        t_frame->set_frame_number(frame_number);

        t_frame->set_t_capture(t_capture.microseconds());
        t_frame->set_t_sent(t_sent.microseconds());
        t_frame->set_t_capture_camera(t_capture_camera.microseconds());
    }
};
} // namespace Immortals::Common
