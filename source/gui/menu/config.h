#pragma once

namespace Tyr::Gui
{
class ConfigMenu
{
public:
    void draw();

private:
    void drawItem(const std::string &t_key, toml::node &t_value);
    void drawArray(toml::array &t_array, int t_columns = 0);
    void drawTable(toml::table &t_table);

    bool m_dirty = false;
};

} // namespace Tyr::Gui
