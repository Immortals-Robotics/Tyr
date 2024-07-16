#include "soccer.h"

namespace Tyr::Gui
{
void SoccerMenu::draw(const Common::Soccer::State &t_state)
{
    std::map<Common::Soccer::Role, int> role_map;
    for (const auto &robot : t_state.robots)
    {
        role_map[robot.role] = robot.id;
    }

    for (const auto& [role, id] : role_map)
    {
        if (role == Common::Soccer::Role::Unknown)
            continue;

        ImGui::SeparatorText(fmt::format("{}", role).c_str());
        ImGui::Text("id: %d", id);
    }
}
} // namespace Tyr::Gui
