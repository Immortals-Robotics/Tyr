#include "soccer.h"

namespace Tyr::Gui
{
void SoccerMenu::draw(const Common::Soccer::State &t_state)
{
    ImGui::SeparatorText("Global");
    ImGui::Text(t_state.is_defending ? "Defending" : "Attacking");
    ImGui::SameLine(0, 2.5f);
    ImGui::Text(" | ");
    ImGui::SameLine(0, 2.5f);
    ImGui::Text("Rnd: %.2f", t_state.random_param);
    if (!t_state.target_str.empty())
    {
        ImGui::SameLine(0, 2.5f);
        ImGui::Text(" | ");
        ImGui::SameLine(0, 2.5f);
        ImGui::Text("str: %s", t_state.target_str.c_str());
    }
    if (t_state.opp_restarted)
    {
        ImGui::SameLine(0, 2.5f);
        ImGui::Text(" | ");
        ImGui::SameLine(0, 2.5f);
        ImGui::Text("Opp Restarted");
    }
    if (t_state.gk_intercepting)
    {
        ImGui::SameLine(0, 2.5f);
        ImGui::Text(" | ");
        ImGui::SameLine(0, 2.5f);
        ImGui::Text("GK Intercepting");
    }

    if (t_state.func_count > 0)
    {
        ImGui::SeparatorText("FSM");
        ImGui::Text("state: %d", t_state.func_state);
        ImGui::SameLine(0, 2.5f);
        ImGui::Text(" | ");
        ImGui::SameLine(0, 2.5f);
        ImGui::Text("count: %d", t_state.func_count);
    }

    std::map<Common::Soccer::Role, int> role_map;
    for (const auto &robot : t_state.robots)
    {
        role_map[robot.role] = robot.id;
    }

    for (const auto& [role, id] : role_map)
    {
        if (role == Common::Soccer::Role::Unknown)
            continue;

        ImGui::SeparatorText(fmt::to_string(role).c_str());

        ImGui::Text("id: %d", id);
        ImGui::SameLine(0, 2.5f);
        ImGui::Text(" | ");
        ImGui::SameLine(0, 2.5f);
        if (t_state.robots[id].navigated)
            ImGui::Text("navigated");
        else if (t_state.robots[id].halted)
            ImGui::Text("halted");

        if (t_state.robots[id].shoot > 0.0f)
        {
            ImGui::SameLine(0, 2.5f);
            ImGui::Text(" | ");
            ImGui::SameLine(0, 2.5f);
            ImGui::Text("shoot: %.2f", t_state.robots[id].shoot);
        }
        if (t_state.robots[id].chip > 0.0f)
        {
            ImGui::SameLine(0, 2.5f);
            ImGui::Text(" | ");
            ImGui::SameLine(0, 2.5f);
            ImGui::Text("chip: %.2f", t_state.robots[id].chip);
        }
        if (t_state.robots[id].dribbler > 0.0f)
        {
            ImGui::SameLine(0, 2.5f);
            ImGui::Text(" | ");
            ImGui::SameLine(0, 2.5f);
            ImGui::Text("dribbler: %.2f", t_state.robots[id].dribbler);
        }

        ImGui::Text("Pass");
        ImGui::SameLine(0, 2.5f);
        ImGui::Text(" | ");
        ImGui::SameLine(0, 2.5f);
        ImGui::Text("%s", fmt::to_string(t_state.robots[id].one_touch_type).c_str());
        ImGui::SameLine(0, 2.5f);
        ImGui::Text(" | ");
        ImGui::SameLine(0, 2.5f);
        ImGui::Text("arriving: %s", t_state.robots[id].one_touch_arriving ? "true" : "false");

        if (t_state.robots[id].mark_target != -1)
            ImGui::Text("mark_target: %d", t_state.robots[id].mark_target);
    }
}
} // namespace Tyr::Gui
