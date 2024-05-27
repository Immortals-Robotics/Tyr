#include "config_menu.h"

namespace Tyr::Gui
{
void ConfigMenu::drawConfigItem(const std::string &t_key, toml::node &t_value)
{
    static int depth = 0;

    depth++;
    ImGui::PushID(t_key.c_str());

    if (t_value.is_table())
    {
        // only put root-level tables inside a tree node
        const bool tree = depth == 1;

        if (!tree || ImGui::TreeNode(t_key.c_str()))
        {
            if (!tree)
                ImGui::SeparatorText(t_key.c_str());

            drawConfigTable(*t_value.as_table());

            if (tree)
                ImGui::TreePop();
        }
    }
    else if (t_value.is_array())
    {
        int columns = 0;
        if (t_key == "use_camera")
            columns = 4;

        if (ImGui::TreeNode(t_key.c_str()))
        {
            drawConfigArray(*t_value.as_array(), columns);
            ImGui::TreePop();
        }
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
        // handle special cases
        if (t_key == "our_side")
        {
            static constexpr std::array kEnums = {"Left", "Right"};
            static_assert(static_cast<int>(Common::TeamSide::Left) == 0);
            static_assert(static_cast<int>(Common::TeamSide::Right) == 1);

            m_configs_dirty |= ImGui::Combo(t_key.c_str(), reinterpret_cast<int *>(&t_value.as_integer()->get()),
                                            kEnums.data(), kEnums.size());
        }
        else if (t_key == "our_color")
        {
            static constexpr std::array kEnums = {"Blue", "Yellow"};
            static_assert(static_cast<int>(Common::TeamColor::Blue) == 0);
            static_assert(static_cast<int>(Common::TeamColor::Yellow) == 1);

            m_configs_dirty |= ImGui::Combo(t_key.c_str(), reinterpret_cast<int *>(&t_value.as_integer()->get()),
                                            kEnums.data(), kEnums.size());
        }
        else
        {
            m_configs_dirty |= ImGui::InputScalar(t_key.c_str(), ImGuiDataType_S64, &t_value.as_integer()->get());
        }
    }
    else if (t_value.is_boolean())
    {
        m_configs_dirty |= ImGui::Checkbox(t_key.c_str(), &t_value.as_boolean()->get());
    }
    else
    {
        Common::logWarning("Config item {} has unsupported config type: {}", t_key, (int) t_value.type());
    }

    depth--;
    ImGui::PopID();
}

void ConfigMenu::drawConfigArray(toml::array &t_array, const int t_columns)
{
    if (t_columns == 0 || ImGui::BeginTable("array", t_columns))
    {
        t_array.for_each(
            [this, t_columns](const int idx, auto &&value)
            {
                if (t_columns)
                    ImGui::TableNextColumn();
                drawConfigItem(std::to_string(idx), value);
            });

        if (t_columns)
            ImGui::EndTable();
    }
}

void ConfigMenu::drawConfigTable(toml::table &t_table)
{
    for (auto &&[key, value] : t_table)
    {
        drawConfigItem(std::string{key.str()}, value);
    }
}

void ConfigMenu::drawConfigTab()
{
    drawConfigTable(Common::config().root());

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

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoDecoration;

    ImGui::SetNextWindowSize(ImVec2(250., (main_window_width - 650.) * 0.77));
    if (ImGui::Begin("Config", nullptr, flags))
    {
        drawTabBar();
        ImGui::End();
    }
}
} // namespace Tyr::Gui
