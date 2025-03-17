#pragma once

#include "robot.h"

namespace Immortals::Common::Soccer
{
class State
{
public:
    State() = default;

    explicit State(const Protos::Immortals::Soccer::State &t_state)
    {
        time = TimePoint::fromMicroseconds(t_state.time());

        for (const auto &robot : t_state.robots())
        {
            robots[robot.id()] = static_cast<Robot>(robot);
        }

        random_param = t_state.random_param();
        target_str   = t_state.target_str();

        is_defending  = t_state.is_defending();
        opp_restarted = t_state.opp_restarted();

        gk_intercepting = t_state.gk_intercepting();

        func_state = t_state.func_state();
        func_count = t_state.func_count();
    }

    void fillProto(Protos::Immortals::Soccer::State *const t_state) const
    {
        t_state->set_time(time.microseconds());

        for (const auto &robot : robots)
        {
            if(robot.role == Role::Unknown)
                continue;

            robot.fillProto(t_state->add_robots());
        }

        t_state->set_random_param(random_param);
        t_state->set_target_str(target_str);

        t_state->set_is_defending(is_defending);
        t_state->set_opp_restarted(opp_restarted);

        t_state->set_gk_intercepting(gk_intercepting);

        t_state->set_func_state(func_state);
        t_state->set_func_count(func_count);
    }

    TimePoint time;

    Robot robots[Config::Common::kMaxRobots];

    float       random_param = 0.0f;
    std::string target_str;

    bool is_defending  = false;
    bool opp_restarted = false;

    bool gk_intercepting = false;

    int func_state = -1;
    int func_count = -1;
};
} // namespace Immortals::Common::Soccer
