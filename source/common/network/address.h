#pragma once

#include "../config/base.h"

namespace Immortals::Common
{
struct NetworkAddress final : Config::IConfig
{
    NetworkAddress() = default;

    NetworkAddress(const std::string_view ip, const unsigned short port) : ip(ip), port(port)
    {}
#if FEATURE_UDP
    explicit NetworkAddress(const asio::ip::udp::endpoint &t_endpoint)
        : ip(t_endpoint.address().to_string()), port(t_endpoint.port())
    {}
#endif

#if FEATURE_CONFIG_FILE
    void load(const toml::node_view<const toml::node> t_node) override
    {
        ip   = t_node["ip"].value_or(ip);
        port = t_node["port"].value_or(port);
    }
#endif

    std::string    ip;
    unsigned short port = 0;
};
} // namespace Immortals::Common

#if FEATURE_LOGGING
template <>
struct fmt::formatter<Immortals::Common::NetworkAddress> : fmt::formatter<std::string>
{
    auto format(Immortals::Common::NetworkAddress t_a, format_context &t_ctx) const
    {
        return fmt::format_to(t_ctx.out(), "{}:{}", t_a.ip, t_a.port);
    }
};
#endif
