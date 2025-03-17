#pragma once

#include "ball.h"
#include "robot.h"

namespace Immortals::Common
{
struct WorldState
{
    TimePoint time;

    BallState ball;

    RobotState own_robot[Config::Common::kMaxRobots];
    RobotState opp_robot[Config::Common::kMaxRobots];

    WorldState()
    {
        for (int i = 0; i < Config::Common::kMaxRobots; i++)
        {
            own_robot[i].vision_id = i;
        }
        for (int i = 0; i < Config::Common::kMaxRobots; i++)
        {
            opp_robot[i].vision_id = i;
        }
    }

    explicit WorldState(const Protos::Immortals::WorldState &t_state)
    {
        time = TimePoint::fromMicroseconds(t_state.time());

        ball = static_cast<BallState>(t_state.ball());

        for (const auto &robot : t_state.own_robot())
        {
            own_robot[robot.id()] = static_cast<RobotState>(robot);
        }

        for (const auto &robot : t_state.opp_robot())
        {
            opp_robot[robot.id()] = static_cast<RobotState>(robot);
        }
    }

    void fillProto(Protos::Immortals::WorldState *const t_state) const
    {
        t_state->set_time(time.microseconds());

        ball.fillProto(t_state->mutable_ball());

        for (const auto &robot : own_robot)
        {
            robot.fillProto(t_state->add_own_robot());
        }

        for (const auto &robot : opp_robot)
        {
            robot.fillProto(t_state->add_opp_robot());
        }
    }
};
} // namespace Immortals::Common
