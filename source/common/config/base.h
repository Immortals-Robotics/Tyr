#pragma once

namespace Immortals::Common::Config
{
#if FEATURE_CONFIG_FILE
template <typename Enum>
void fillEnum(const toml::node_view<const toml::node> t_node, Enum &t_enum)
{
    t_enum = static_cast<Enum>(t_node.value_or(static_cast<int>(t_enum)));
}
#endif

struct IConfig
{
protected:
    virtual ~IConfig() = default;

#if FEATURE_CONFIG_FILE
public:
    virtual void load(toml::node_view<const toml::node> t_node) = 0;
#endif
};
} // namespace Immortals::Common::Config
