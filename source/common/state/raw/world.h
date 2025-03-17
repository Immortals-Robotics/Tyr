#pragma once

#include "ball.h"
#include "frame.h"
#include "robot.h"

namespace Immortals::Common
{
struct RawWorldState
{
    TimePoint time;

    std::vector<RawFrame> frames;

    std::vector<RawBallState> balls;

    std::vector<RawRobotState> yellow_robots;
    std::vector<RawRobotState> blue_robots;

    RawWorldState() = default;

    explicit RawWorldState(const Protos::Immortals::RawWorldState &t_state)
    {
        time = TimePoint::fromMicroseconds(t_state.time());

        for (const auto &frame : t_state.frames())
        {
            frames.emplace_back(frame);
        }

        for (const auto &ball : t_state.balls())
        {
            balls.emplace_back(ball, frames);
        }

        for (const auto &robot : t_state.yellow_robots())
        {
            yellow_robots.emplace_back(robot, frames);
        }

        for (const auto &robot : t_state.blue_robots())
        {
            blue_robots.emplace_back(robot, frames);
        }
    }

    void fillProto(Protos::Immortals::RawWorldState *const t_state) const
    {
        t_state->set_time(time.microseconds());

        for (const auto &frame : frames)
        {
            frame.fillProto(t_state->add_frames());
        }

        for (const auto &ball : balls)
        {
            ball.fillProto(t_state->add_balls());
        }

        for (const auto &robot : yellow_robots)
        {
            robot.fillProto(t_state->add_yellow_robots());
        }

        for (const auto &robot : blue_robots)
        {
            robot.fillProto(t_state->add_blue_robots());
        }
    }

    void addFrame(const Protos::Ssl::Vision::DetectionFrame &t_frame)
    {
        const unsigned frame_idx = frames.size();
        frames.emplace_back(t_frame);

        for (const auto &ball : t_frame.balls())
        {
            balls.emplace_back(ball, frames, frame_idx);
        }

        for (const auto &robot : t_frame.robots_yellow())
        {
            yellow_robots.emplace_back(robot, TeamColor::Yellow, frames, frame_idx);
        }

        for (const auto &robot : t_frame.robots_blue())
        {
            blue_robots.emplace_back(robot, TeamColor::Blue, frames, frame_idx);
        }
    }

    void clear()
    {
        time = TimePoint{};

        frames.clear();
        balls.clear();
        yellow_robots.clear();
        blue_robots.clear();
    }
};
} // namespace Immortals::Common
