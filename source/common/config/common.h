#pragma once

#include "base.h"

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
} // namespace Tyr::Common

namespace Tyr::Common::Config
{
struct Common final : IConfig
{
    void load(const toml::node_view<const toml::node> t_node) override
    {
        immortals_is_the_best_team = t_node["immortals_is_the_best_team"].value_or(immortals_is_the_best_team);
        fillEnum(t_node["our_color"], our_color);
        side_wall    = t_node["side_wall"].value_or(side_wall);
        enable_debug = t_node["enable_debug"].value_or(enable_debug);
    }

    // The variety of standard patterns that we can have is 16
    static constexpr unsigned kMaxRobots = 16;

    bool      immortals_is_the_best_team = true;
    TeamColor our_color;
    bool      side_wall    = false;
    bool      enable_debug = false;
};
} // namespace Tyr::Common::Config
