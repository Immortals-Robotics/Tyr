#pragma once

#include "base.h"

namespace Immortals::Common::Config
{
struct RobotPhysicalStatus final : IConfig
{
    RobotPhysicalStatus() = default;

#if FEATURE_CONFIG_FILE
    void load(const toml::node_view<const toml::node> t_node) override
    {
        id              = t_node["id"].value_or(id);
        has_dribbler    = t_node["has_dribbler"].value_or(has_dribbler);
        has_direct_kick = t_node["has_direct_kick"].value_or(has_direct_kick);
        has_chip_kick   = t_node["has_chip_kick"].value_or(has_chip_kick);
        is_3D_printed   = t_node["is_3D_printed"].value_or(is_3D_printed);
    }
#endif

    int  id              = -1;
    bool has_dribbler    = false;
    bool has_direct_kick = false;
    bool has_chip_kick   = false;
    bool is_3D_printed   = false;
};

struct VelocityProfile final : IConfig
{
    VelocityProfile() = default;

#if FEATURE_CONFIG_FILE
    void load(const toml::node_view<const toml::node> t_node) override
    {
        speed        = t_node["speed"].value_or(speed);
        acceleration = t_node["acceleration"].value_or(acceleration);
    }
#endif

    float speed        = 0.0f;
    float acceleration = 0.0f;
};

struct Soccer final : IConfig
{
#if FEATURE_CONFIG_FILE
    void load(const toml::node_view<const toml::node> t_node) override
    {
        one_touch_beta    = t_node["one_touch_beta"].value_or(one_touch_beta);
        one_touch_gamma   = t_node["one_touch_gamma"].value_or(one_touch_gamma);
        one_touch_shoot_k = t_node["one_touch_shoot_k"].value_or(one_touch_shoot_k);

        gk_tight_start_angle   = t_node["gk_tight_start_angle"].value_or(gk_tight_start_angle);
        def_tight_start_angle  = t_node["def_tight_start_angle"].value_or(def_tight_start_angle);
        def_prediction_time    = t_node["def_prediction_time"].value_or(def_prediction_time);
        def_max_extension_area = t_node["def_max_extension_area"].value_or(def_max_extension_area);
        def_ball_distance_coef = t_node["def_ball_distance_coef"].value_or(def_ball_distance_coef);

        kick_tune_coef   = t_node["kick_tune_coef"].value_or(kick_tune_coef);
        chip_tune_coef   = t_node["chip_tune_coef"].value_or(chip_tune_coef);

        mark_in_stop = t_node["mark_in_stop"].value_or(mark_in_stop);
        mark_distance = t_node["mark_distance"].value_or(mark_distance);

        penalty_area_mark = t_node["penalty_area_mark"].value_or(penalty_area_mark);
        penalty_area_mark_distance = t_node["penalty_area_mark_distance"].value_or(penalty_area_mark_distance);

        if (auto *robot_physical_status_array = t_node["robot_physical_status"].as_array())
        {
            for (size_t i = 0; i < robot_physical_status_array->size(); i++)
            {
                RobotPhysicalStatus &status = robot_physical_status[i];

                status.load(t_node["robot_physical_status"][i]);

                if (status.id == -1)
                {
                    logError("Robot ID for #{} is not defined in the .toml config file", i);
                }
                if (status.id != i)
                {
                    // This check is to make sure the config file is written in order so
                    // less mistake will happen during the match setup
                    logError("Robot ID is out of order in iteration #{} the id should is {} but should be {}", i,
                             status.id, i);
                }
            }
        }

        if (auto *velocity_profiles = t_node["velocity_profile"].as_table())
        {
            velocity_profile_sooski.load((*velocity_profiles)["sooski"]);
            velocity_profile_aroom.load((*velocity_profiles)["aroom"]);
            velocity_profile_mamooli.load((*velocity_profiles)["mamooli"]);
            velocity_profile_kharaki.load((*velocity_profiles)["kharaki"]);
        }
    }
#endif

    RobotPhysicalStatus robot_physical_status[Common::kMaxRobots];

    VelocityProfile velocity_profile_sooski;
    VelocityProfile velocity_profile_aroom;
    VelocityProfile velocity_profile_mamooli;
    VelocityProfile velocity_profile_kharaki;

    float one_touch_beta         = 0.4f;  // Damping factor
    float one_touch_gamma        = 0.14f; // Reflect factor
    float one_touch_shoot_k      = 4000.0f;
    float gk_tight_start_angle   = 20.0f;
    float def_tight_start_angle  = 40.0f;
    float def_prediction_time    = 0.5f;
    float def_max_extension_area = 1100.0f;
    float def_ball_distance_coef = 0.7f;
    float kick_tune_coef = 1.f;
    float chip_tune_coef = 1.f;
    bool mark_in_stop = false;
    float mark_distance = 500.0f;
    bool penalty_area_mark = false;
    float penalty_area_mark_distance = 120.0f;
};
} // namespace Immortals::Common::Config
