#pragma once

#include "../network/address.h"
#include "base.h"

namespace Immortals::Common::Config
{
struct Network final : IConfig
{
#if FEATURE_CONFIG_FILE
    void load(const toml::node_view<const toml::node> t_node) override
    {

        use_simulated_vision = t_node["active"]["use_simulated_vision"].value_or(use_simulated_vision);
        use_internal_referee = t_node["active"]["use_internal_referee"].value_or(use_internal_referee);

        use_simulated_vision ? vision_address.load(t_node["vision_sim"]) :  vision_address.load(t_node["vision"]);
        
        tracker_address.load(t_node["tracker"]);

        use_internal_referee ? referee_address.load(t_node["internal_referee"]) : referee_address.load(t_node["referee"]);

        strategy_address.load(t_node["strategy"]);

        sender_address.load(t_node["sender"]);
        grsim_address.load(t_node["grsim"]);

        control_simulation_address.load(t_node["control_simulation"]);
        blue_robot_simulation_address.load(t_node["blue_robot_simulation"]);
        yellow_robot_simulation_address.load(t_node["yellow_robot_simulation"]);
    }
#endif

    static constexpr size_t kMaxUdpPacketSize = 16 * 1024; // TODO what should the size be really?

    NetworkAddress vision_address  = {"224.5.23.2", 10006};
    NetworkAddress tracker_address = {"224.5.23.2", 10010};

    NetworkAddress referee_address = {"224.5.23.1", 10003};

    NetworkAddress strategy_address = {"224.5.23.3", 60006};

    NetworkAddress sender_address = {"224.5.92.5", 60005};
    NetworkAddress grsim_address  = {"127.0.0.1", 20011};

    NetworkAddress control_simulation_address      = {"127.0.0.1", 10300};
    NetworkAddress blue_robot_simulation_address   = {"127.0.0.1", 10301};
    NetworkAddress yellow_robot_simulation_address = {"127.0.0.1", 10302};

    bool use_simulated_vision = false;
    bool use_internal_referee = false;
    
    // NNG urls
    std::string raw_world_state_url = "inproc://raw_world_state";
    std::string world_state_url     = "inproc://world_state";
    std::string debug_url           = "inproc://debug";
    std::string referee_state_url   = "inproc://referee_state";
    std::string soccer_state_url    = "inproc://soccer_state";
    std::string commands_url        = "inproc://commands";

    // DB names
    std::string raw_world_state_db = "raw_world_state";
    std::string world_state_db     = "world_state";
    std::string debug_db           = "debug";
    std::string referee_db         = "referee";
    std::string soccer_db          = "soccer";
};
} // namespace Immortals::Common::Config
