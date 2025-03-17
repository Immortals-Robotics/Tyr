#pragma once

#include "base.h"

namespace Immortals::Common::Config
{
struct Vision final : IConfig
{
#if FEATURE_CONFIG_FILE
    void load(const toml::node_view<const toml::node> t_node) override
    {
        if (auto *use_camera_array = t_node["use_camera"].as_array())
        {
            for (unsigned cam_idx = 0; cam_idx < kCamCount; ++cam_idx)
            {
                use_camera[cam_idx] = (*use_camera_array)[cam_idx].value_or(use_camera[cam_idx]);
            }
        }

        vision_frame_rate = t_node["vision_frame_rate"].value_or(vision_frame_rate);
        predict_time = t_node["predict_time"].value_or(predict_time);

        max_ball_hist = t_node["max_ball_hist"].value_or(max_ball_hist);

        merge_distance        = t_node["merge_distance"].value_or(merge_distance);
        ball_merge_distance   = t_node["ball_merge_distance"].value_or(ball_merge_distance);
        max_ball_2_frame_dist = t_node["max_ball_2_frame_dist"].value_or(max_ball_2_frame_dist);

        max_robot_frame_not_seen = t_node["max_robot_frame_not_seen"].value_or(max_robot_frame_not_seen);
        max_ball_frame_not_seen  = t_node["max_ball_frame_not_seen"].value_or(max_ball_frame_not_seen);

        camera_delay     = t_node["camera_delay"].value_or(camera_delay);
        kick_threshold   = t_node["kick_threshold"].value_or(kick_threshold);
        chip_max_error   = t_node["chip_max_error"].value_or(chip_max_error);
        chip_min_records = t_node["chip_min_records"].value_or(chip_min_records);
        chip_max_records = t_node["chip_max_records"].value_or(chip_max_records);
        chip_max_vel_z   = t_node["chip_max_vel_z"].value_or(chip_max_vel_z);
        kicker_depth     = t_node["kicker_depth"].value_or(kicker_depth);
        use_ball_3d      = t_node["use_ball_3d"].value_or(use_ball_3d);
    }
#endif

    static constexpr unsigned kCamCount = 8;
    static constexpr unsigned kMaxBalls = 10;

    float vision_frame_rate = 60.0f;
    float predict_time = 0.12f;

    std::array<bool, kCamCount> use_camera = {};

    int max_ball_hist = 10;

    float merge_distance        = 70.0;
    float ball_merge_distance   = 70.0;
    float max_ball_2_frame_dist = 1000.0;

    int max_robot_frame_not_seen = 200;
    int max_ball_frame_not_seen  = 120; // 2 seconds

    float camera_delay        = 0.0;
    float kick_threshold      = 0.0;
    float chip_max_error      = 300000;
    int   chip_min_records    = 3;
    int   chip_max_records    = 200;
    float chip_max_vel_z      = 7000.;
    float kicker_depth        = 150.0;
    bool  use_ball_3d         = true;
};
} // namespace Immortals::Common::Config
