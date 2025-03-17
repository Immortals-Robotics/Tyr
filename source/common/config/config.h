#pragma once

#if FEATURE_CONFIG_FILE
#include "file.h"
#endif

#include "common.h"
#include "network.h"
#include "soccer.h"
#include "vision.h"

namespace Immortals::Common::Config
{
class Config
{
private:
#if FEATURE_CONFIG_FILE
    Config(const std::filesystem::path &t_path) : m_file(t_path)
    {}
#else
    Config() = default;
#endif

    ~Config() = default;

    friend struct ::Immortals::Common::Services;

#if FEATURE_CONFIG_FILE
    File m_file;
#endif

public:
    Config(const Config &)            = delete;
    Config &operator=(const Config &) = delete;

#if FEATURE_CONFIG_FILE
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
#endif

    Common  common;
    Network network;
    Vision  vision;
    Soccer  soccer;
};
} // namespace Immortals::Common::Config
