#pragma once

#include "base.h"

namespace Tyr::Common::Config
{
struct Network final : IConfig
{
    struct Address final : IConfig
    {
        Address() = default;

        Address(const std::string_view ip, const unsigned short port) : ip(ip), port(port)
        {}

        void load(const toml::node_view<const toml::node> t_node) override
        {
            ip   = t_node["ip"].value_or(ip);
            port = t_node["port"].value_or(port);
        }

        std::string    ip;
        unsigned short port = 0;
    };

    Network() = default;

    void load(const toml::node_view<const toml::node> t_node) override
    {
        tracker_address.load(t_node["tracker"]);
        vision_interface_address.load(t_node["vision_interface"]);

        vision_address.load(t_node["vision"]);

        referee_address.load(t_node["referee"]);

        sender_address.load(t_node["sender"]);

        commands_address.load(t_node["commands"]);

        strategy_address.load(t_node["strategy"]);

        grsim_address.load(t_node["grsim"]);

        control_simulation_address.load(t_node["control_simulation"]);
        blue_robot_simulation_address.load(t_node["blue_robot_simulation"]);
        yellow_robot_simulation_address.load(t_node["yellow_robot_simulation"]);

        nrf_frq = t_node["nrf_frq"].value_or(nrf_frq);
    }

    static constexpr size_t kMaxUdpPacketSize = 1024 * 16; // TODO what should the size be really?

    Address vision_address = {"224.5.23.2", 10006};

    Address tracker_address          = {"224.5.23.2", 10010};
    Address vision_interface_address = {"127.0.0.1", 10067};

    Address referee_address = {"224.5.23.1", 10003};

    Address sender_address = {"224.5.92.5", 60005};

    Address commands_address = {"224.5.92.6", 60007};

    Address strategy_address = {"224.5.23.3", 60006};

    Address grsim_address = {"127.0.0.1", 20011};

    Address control_simulation_address      = {"127.0.0.1", 10300};
    Address blue_robot_simulation_address   = {"127.0.0.1", 10301};
    Address yellow_robot_simulation_address = {"127.0.0.1", 10302};

    // NNG urls
    std::string raw_world_state_url = "inproc://raw_world_state";
    std::string world_state_url     = "inproc://world_state";
    std::string debug_url           = "inproc://debug";
    std::string referee_state_url   = "inproc://referee_state";
    std::string commands_url        = "inproc://commands";

    // DB names
    std::string raw_world_state_db = "raw_world_state";
    std::string world_state_db     = "world_state";
    std::string debug_db           = "debug";
    std::string referee_db         = "referee";

    unsigned char nrf_frq; // TODO: unused
};
} // namespace Tyr::Common::Config
