#pragma once

#include "base.h"

namespace Tyr::Common::Config
{
struct RobotPhysicalStatus
{
    bool has_dribbler    = true;
    bool has_direct_kick = true;
    bool has_chip_kick   = true;
    bool is_3D_printed   = false;
};

struct Soccer final : IConfig
{
    void load(const toml::node_view<const toml::node> t_node) override
    {
        fillEnum(t_node["our_side"], our_side);

        command_opp_robots = t_node["command_opp_robots"].value_or(command_opp_robots);
        enable_simulation  = t_node["enable_simulation"].value_or(enable_simulation);

        // Gamma represents the relation between the robots velocity parameter and the velocity in mm/s
        // Gamma is 48 in the GR Simulation
        // and 19 in reality
        // Velocity Gamma
        gamma_reality    = t_node["gamma_reality"].value_or(gamma_reality);
        gamma_simulation = t_node["gamma_simulation"].value_or(gamma_simulation);
        // Angular Gamma
        ang_gamma_reality    = t_node["ang_gamma_reality"].value_or(ang_gamma_reality);
        ang_gamma_simulation = t_node["ang_gamma_simulation"].value_or(ang_gamma_simulation);

        one_touch_beta    = t_node["one_touch_beta"].value_or(one_touch_beta);
        one_touch_gamma   = t_node["one_touch_gamma"].value_or(one_touch_gamma);
        one_touch_shoot_k = t_node["one_touch_shoot_k"].value_or(one_touch_shoot_k);

        gamma_simulation = t_node["gamma_simulation"].value_or(gamma_simulation);

        init_gk_id = t_node["init_gk_id"].value_or(init_gk_id);

        if (auto *robot_physical_status_array = t_node["robot_physical_status"].as_array())
        {
            for (size_t i = 0; i < robot_physical_status_array->size(); i++)
            {
                int id = t_node["robot_physical_status"][i]["id"].value_or(-1);
                if (id == -1)
                {
                    logError("Robot ID for #{} is not defined in the .toml config file", i);
                }
                if (id != i)
                {
                    // This check is to make sure the config file is written in order so
                    // less mistake will happen during the match setup
                    logError("Robot ID is out of order in iteration #{} the id should is {} but should be {}", i, id,
                             i);
                }
                robot_physical_status[id].has_dribbler =
                    t_node["robot_physical_status"][i]["has_dribbler"].value_or(false);
                robot_physical_status[id].has_direct_kick =
                    t_node["robot_physical_status"][i]["has_direct_kick"].value_or(false);
                robot_physical_status[id].has_chip_kick =
                    t_node["robot_physical_status"][i]["has_chip_kick"].value_or(false);
                robot_physical_status[id].is_3D_printed =
                    t_node["robot_physical_status"][i]["is_3D_printed"].value_or(false);
            }
        }
    }

    TeamSide our_side = TeamSide::Left;

    bool command_opp_robots = false;
    bool enable_simulation  = false;

    RobotPhysicalStatus robot_physical_status[Common::kMaxRobots];

    float gamma_reality        = 19;
    float gamma_simulation     = 900;
    float ang_gamma_reality    = 1;
    float ang_gamma_simulation = 1;

    float one_touch_beta    = 0.4f;  // Damping factor
    float one_touch_gamma   = 0.14f; // Reflect factor
    float one_touch_shoot_k = 4000.0f;

    bool mark_in_stop = false;

    int init_gk_id = 0;
};
} // namespace Tyr::Common::Config
