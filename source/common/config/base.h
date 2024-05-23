#pragma once

namespace Tyr::Common::Config
{
template <typename Enum>
void fillEnum(const toml::node_view<const toml::node> t_node, Enum &t_enum)
{
    t_enum = static_cast<Enum>(t_node.value_or(static_cast<int>(t_enum)));
}

struct IConfig
{
protected:
    virtual ~IConfig() = default;

public:
    virtual void load(toml::node_view<const toml::node> t_node) = 0;
};
} // namespace Tyr::Common::Config
