#pragma once

namespace Tyr::Gui
{
class ConfigMenu
{
public:
    void draw();

private:
    void drawConfigItem(const std::string &t_key, toml::node &t_value);
    void drawConfigArray(toml::array &t_array, int t_columns = 0);
    void drawConfigTable(toml::table &t_table);

    bool m_configs_dirty = false;
};

} // namespace Tyr::Gui
