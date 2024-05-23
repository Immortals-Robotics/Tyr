#pragma once

#include "base.h"

namespace Tyr::Common::Config
{
struct Vision final : IConfig
{
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

        max_ball_hist = t_node["max_ball_hist"].value_or(max_ball_hist);

        merge_distance        = t_node["merge_distance"].value_or(merge_distance);
        ball_merge_distance   = t_node["ball_merge_distance"].value_or(ball_merge_distance);
        max_ball_2_frame_dist = t_node["max_ball_2_frame_dist"].value_or(max_ball_2_frame_dist);

        max_robot_frame_not_seen = t_node["max_robot_frame_not_seen"].value_or(max_robot_frame_not_seen);
        max_ball_frame_not_seen  = t_node["max_ball_frame_not_seen"].value_or(max_ball_frame_not_seen);

        use_kalman_pos = t_node["use_kalman_pos"].value_or(use_kalman_pos);
        use_kalman_ang = t_node["use_kalman_ang"].value_or(use_kalman_ang);
    }

    static constexpr unsigned kCamCount = 8;
    static constexpr unsigned kMaxBalls = 10;

    float vision_frame_rate = 60.0f;

    std::array<bool, kCamCount> use_camera = {};

    int max_ball_hist = 10;

    float merge_distance        = 70.0;
    float ball_merge_distance   = 70.0;
    float max_ball_2_frame_dist = 1000.0;

    int max_robot_frame_not_seen = 200;
    int max_ball_frame_not_seen  = 120; // 2 seconds

    bool use_kalman_pos = true;
    bool use_kalman_ang = true; // TODO: check if this is in serious need in reality
};
} // namespace Tyr::Common::Config
