#include "setting.h"

namespace Tyr::Common
{
void Setting::load(const toml::node_view<const toml::node> t_node)
{
    const auto common = t_node["common"];

    immortals_is_the_best_team = common["immortals_is_the_best_team"].value_or(immortals_is_the_best_team);

    fillEnum(common["our_color"], our_color);

    side_wall = common["side_wall"].value_or(side_wall);

    enable_debug = common["enable_debug"].value_or(enable_debug);

    const auto network = t_node["network"];

    tracker_address.load(network["tracker"]);
    debug_address.load(network["debug"]);
    vision_interface_address.load(network["vision_interface"]);

    vision_address.load(network["vision"]);

    referee_address.load(network["referee"]);

    sender_address.load(network["sender"]);

    commands_address.load(network["commands"]);
    // sender_rec_address.load(network["referee"]);

    control_simulation_address.load(network["control_simulation"]);
    blue_robot_simulation_address.load(network["blue_robot_simulation"]);
    yellow_robot_simulation_address.load(network["yellow_robot_simulation"]);

    // vision
    const auto vision = t_node["vision"];

    if (auto *use_camera_array = vision["use_camera"].as_array())
    {
        for (unsigned cam_idx = 0; cam_idx < kCamCount; ++cam_idx)
        {
            use_camera[cam_idx] = (*use_camera_array)[cam_idx].value_or(use_camera[cam_idx]);
        }
    }

    vision_frame_rate = vision["vision_frame_rate"].value_or(vision_frame_rate);

    max_ball_hist = vision["max_ball_hist"].value_or(max_ball_hist);

    merge_distance        = vision["merge_distance"].value_or(merge_distance);
    ball_merge_distance   = vision["ball_merge_distance"].value_or(ball_merge_distance);
    max_ball_2_frame_dist = vision["max_ball_2_frame_dist"].value_or(max_ball_2_frame_dist);

    max_robot_frame_not_seen = vision["max_robot_frame_not_seen"].value_or(max_robot_frame_not_seen);
    max_ball_frame_not_seen  = vision["max_ball_frame_not_seen"].value_or(max_ball_frame_not_seen);

    use_kalman_pos = vision["use_kalman_pos"].value_or(use_kalman_pos);
    use_kalman_ang = vision["use_kalman_ang"].value_or(use_kalman_ang);

    // soccer
    const auto soccer = t_node["soccer"];

    fillEnum(soccer["our_side"], our_side);

    command_opp_robots = soccer["command_opp_robots"].value_or(command_opp_robots);
    enable_simulation  = soccer["enable_simulation"].value_or(enable_simulation);

    nrf_frq = soccer["nrf_frq"].value_or(nrf_frq);

    field_width  = soccer["field_width"].value_or(field_width);
    field_length = soccer["field_length"].value_or(field_length);
    goal_length  = soccer["goal_length"].value_or(goal_length);

    // Gamma represents the relation between the robots velocity parameter and the velocity in mm/s
    // Gamma is 48 in the GR Simulation
    // and 19 in reality
    // Velocity Gamma
    gamma_reality    = soccer["gamma_reality"].value_or(gamma_reality);
    gamma_simulation = soccer["gamma_simulation"].value_or(gamma_simulation);
    // Angular Gamma
    ang_gamma_reality    = soccer["ang_gamma_reality"].value_or(ang_gamma_reality);
    ang_gamma_simulation = soccer["ang_gamma_simulation"].value_or(ang_gamma_simulation);

    robot_behavior_tree_config_filename =
        soccer["robot_behavior_tree_config_filename"].value_or(robot_behavior_tree_config_filename);

    init_gk_id = soccer["init_gk_id"].value_or(init_gk_id);

    if (auto *robot_physical_status_array = soccer["robot_physical_status"].as_array())
    {
        logDebug("Array size of received robot_physical_status: {}", robot_physical_status_array->size());
        for (int i = 0; i < robot_physical_status_array->size(); i++)
        {
            int id = soccer["robot_physical_status"][i]["id"].value_or(-1);
            if (id == -1)
            {
                logError("Robot ID for #{} is not defined in the .toml config file", i);
            }
            if (id != i)
            {
                // This check is to make sure the config file is written in order so
                // less mistake will happen during the match setup
                logError("Robot ID is out of order in iteration #{} the id should is {} but should be {}", i, id, i);
            }
            robot_physical_status[id].has_dribbler = soccer["robot_physical_status"][i]["has_dribbler"].value_or(false);
            robot_physical_status[id].has_direct_kick =
                soccer["robot_physical_status"][i]["has_direct_kick"].value_or(false);
            robot_physical_status[id].has_chip_kick =
                soccer["robot_physical_status"][i]["has_chip_kick"].value_or(false);
            robot_physical_status[id].is_3D_printed =
                soccer["robot_physical_status"][i]["is_3D_printed"].value_or(false);

            logDebug("Robot #{} status:", id);
            logDebug(" -has_dribbler: {}", robot_physical_status[id].has_dribbler);
            logDebug(" -has_direct_kick: {}", robot_physical_status[id].has_direct_kick);
            logDebug(" -has_chip_kick: {}", robot_physical_status[id].has_chip_kick);
            logDebug(" -is_3D_printed: {}", robot_physical_status[id].is_3D_printed);
        }
    }
}
} // namespace Tyr::Common
