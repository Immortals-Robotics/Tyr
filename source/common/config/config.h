#pragma once

#include "file.h"

#include "common.h"
#include "network.h"
#include "soccer.h"
#include "vision.h"

namespace Tyr::Common::Config
{
class Config
{
private:
    Config(const std::filesystem::path &t_file_name) : m_file(t_file_name)
    {}

    ~Config() = default;

    friend struct ::Tyr::Common::Services;

    File m_file;

public:
            Config(const Config &)    = delete;
    Config &operator=(const Config &) = delete;

    void load()
    {
        m_file.load();

        const toml::table &root = m_file.root();

        common.load(root["common"]);
        network.load(root["network"]);
        vision.load(root["vision"]);
        soccer.load(root["soccer"]);
    }

    void save() const
    {
        m_file.save();
    }

    const toml::table &root() const
    {
        return m_file.root();
    }

    toml::table &root()
    {
        return m_file.root();
    }

    Common  common;
    Network network;
    Vision  vision;
    Soccer  soccer;
};
} // namespace Tyr::Common::Config
