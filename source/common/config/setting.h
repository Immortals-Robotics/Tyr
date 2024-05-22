#pragma once

#include "file.h"

#include "base.h"
#include "common.h"
#include "network.h"
#include "soccer.h"
#include "vision.h"

namespace Tyr::Common::Config
{
class Setting final : IConfig
{
private:
    Setting() = default;

    ~Setting() override = default;

    void load(toml::node_view<const toml::node> t_table) override
    {
        common.load(t_table["common"]);
        network.load(t_table["network"]);
        vision.load(t_table["vision"]);
        soccer.load(t_table["soccer"]);
    }

    friend struct Services;

public:
             Setting(const Setting &)   = delete;
    Setting &operator=(const Setting &) = delete;

    Common  common;
    Network network;
    Vision  vision;
    Soccer  soccer;
};
} // namespace Tyr::Common::Config
