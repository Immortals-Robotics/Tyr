#include "config_menu.h"

namespace Tyr::Gui
{
ConfigMenu::ConfigMenu()
{
    m_window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoDecoration;
}

void ConfigMenu::drawConfigItem(const std::string &t_key, toml::node &t_value)
{
    if (t_value.is_table())
    {
        if (ImGui::TreeNode(t_key.c_str()))
        {
            drawConfigTable(t_value.as_table());
            ImGui::TreePop();
        }
    }
    else if (t_value.is_array())
    {
        ImGui::SeparatorText(t_key.c_str());
        drawConfigArray(t_value.as_array());
        ImGui::Separator();
        ImGui::Spacing();
    }
    else if (t_value.is_string())
    {
        m_configs_dirty |= ImGui::InputText(t_key.c_str(), &t_value.as_string()->get());
    }
    else if (t_value.is_floating_point())
    {
        m_configs_dirty |= ImGui::InputScalar(t_key.c_str(), ImGuiDataType_Double, &t_value.as_floating_point()->get());
    }
    else if (t_value.is_integer())
    {
        m_configs_dirty |= ImGui::InputScalar(t_key.c_str(), ImGuiDataType_S64, &t_value.as_integer()->get());
    }
    else if (t_value.is_boolean())
    {
        m_configs_dirty |= ImGui::Checkbox(t_key.c_str(), &t_value.as_boolean()->get());
    }
    else
    {
        Common::logWarning("Config item {} has unsupported config type: {}", t_key, (int) t_value.type());
    }
}

void ConfigMenu::drawConfigArray(toml::array *const t_array)
{
    t_array->for_each([this](const int idx, auto &&value) { drawConfigItem(std::to_string(idx), value); });
}

void ConfigMenu::drawConfigTable(toml::table *const t_table)
{
    for (auto &&[key, value] : *t_table)
    {
        drawConfigItem(std::string{key.str()}, value);
    }
}

void ConfigMenu::drawConfigTab()
{
    drawConfigTable(&Common::config().root());

    if (m_configs_dirty)
    {
        Common::Services::config().save();
        Common::Services::config().load();

        m_configs_dirty = false;
    }
}

void ConfigMenu::drawTabBar()
{
    if (ImGui::BeginTabBar("Config tabs"))
    {
        if (ImGui::BeginTabItem("Config"))
        {
            drawConfigTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Debug Filter"))
        {
            drawFilterTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void ConfigMenu::draw()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    auto main_window_height = GetScreenHeight();
    auto main_window_width  = GetScreenWidth();
    if (((main_window_width - 650.) * 0.77) >= main_window_height - 200.)
    {
        main_window_width = (main_window_height - 200.) / 0.77 + 650.;
    }

    ImGui::SetNextWindowSize(ImVec2(250., (main_window_width - 650.) * 0.77));
    if (ImGui::Begin("Config", nullptr, m_window_flags))
    {
        drawTabBar();
        ImGui::End();
    }
}
} // namespace Tyr::Gui
