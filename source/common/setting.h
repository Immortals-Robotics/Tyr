#pragma once

#include "config/config.h"

namespace Tyr::Common
{
enum class TeamColor
{
    Blue   = 0,
    Yellow = 1,
};

inline TeamColor operator!(const TeamColor t_color)
{
    return t_color == TeamColor::Blue ? TeamColor::Yellow : TeamColor::Blue;
}

enum class TeamSide
{
    Left  = 0,
    Right = 1,
};

struct NetworkAddress
{
    NetworkAddress() = default;

    NetworkAddress(const std::string_view ip, const unsigned short port) : ip(ip), port(port)
    {}

    void load(toml::node_view<const toml::node> t_node)
    {
        ip   = t_node["ip"].value_or(ip);
        port = t_node["port"].value_or(port);
    }

    std::string    ip;
    unsigned short port = 0;
};

template <typename Enum>
void fillEnum(toml::node_view<const toml::node> t_node, Enum &t_enum)
{
    t_enum = static_cast<Enum>(t_node.value_or(static_cast<int>(t_enum)));
}

struct RobotPhysicalStatus
{
    bool has_dribbler    = true;
    bool has_direct_kick = true;
    bool has_chip_kick   = true;
    bool is_3D_printed   = false;
};

struct Setting : IConfig
{
private:
    Setting()  = default;
    ~Setting() = default;

    void        load(toml::table t_table) override;
    toml::table m_config_table;
    friend struct Services;

public:
    Setting(const Setting &)            = delete;
    Setting &operator=(const Setting &) = delete;

    template <typename T>
    void updateSetting(const std::string &_settings_key, const T &_new_value);

    toml::table             getConfigTable(void);
    static constexpr size_t kMaxUdpPacketSize = 1024 * 16; // TODO what should the size be really?

    // The variety of standard patterns that we can have is 16
    static constexpr unsigned kMaxRobots = 16;

    bool immortals_is_the_best_team = true;

    bool enable_debug = false;

    double vision_frame_rate = 60.0;

    TeamColor our_color;

    bool side_wall = false;

    NetworkAddress tracker_address          = {"224.5.23.2", 10010};
    NetworkAddress vision_interface_address = {"127.0.0.1", 10067};

    // vision
    static constexpr unsigned kCamCount = 8;
    static constexpr unsigned kMaxBalls = 10;

    NetworkAddress vision_address = {"224.5.23.2", 10006};

    std::array<bool, kCamCount> use_camera = {};

    int max_ball_hist = 10;

    double merge_distance        = 70.0;
    double ball_merge_distance   = 70.0;
    double max_ball_2_frame_dist = 1000.0;

    int max_robot_frame_not_seen = 200;
    int max_ball_frame_not_seen  = 120; // 2 seconds

    bool use_kalman_pos = true;
    bool use_kalman_ang = true; // TODO: check if this is in serious need in reality

    // soccer
    static constexpr int kMaxOnFieldTeamRobots = 8;

    NetworkAddress referee_address = {"224.5.23.1", 10003};

    NetworkAddress sender_address     = {"224.5.92.5", 60005};
    NetworkAddress sender_rec_address = {"", 0}; // TODO: unused?

    NetworkAddress commands_address = {"224.5.92.6", 60007};

    NetworkAddress strategy_address = {"224.5.23.3", 60006};

    NetworkAddress grsim_address = {"127.0.0.1", 20011};

    NetworkAddress control_simulation_address      = {"127.0.0.1", 10300};
    NetworkAddress blue_robot_simulation_address   = {"127.0.0.1", 10301};
    NetworkAddress yellow_robot_simulation_address = {"127.0.0.1", 10302};

    // NNG urls
    std::string raw_world_state_url = "inproc://raw_world_state";
    std::string world_state_url     = "inproc://world_state";
    std::string debug_url           = "inproc://debug";
    std::string referee_state_url   = "inproc://referee_state";
    std::string commands_url        = "inproc://commands";

    TeamSide our_side = TeamSide::Left;

    bool command_opp_robots = false;
    bool enable_simulation  = false;

    unsigned char nrf_frq; // TODO: unused

    int init_gk_id = 0;

    RobotPhysicalStatus robot_physical_status[kMaxRobots];

    double gamma_reality        = 19;
    double gamma_simulation     = 900;
    double ang_gamma_reality    = 1;
    double ang_gamma_simulation = 1;

    std::string robot_behavior_tree_config_filename = "bt_default.xml";
};
} // namespace Tyr::Common
