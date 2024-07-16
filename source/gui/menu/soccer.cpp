#include "soccer.h"

namespace Tyr::Gui
{
void SoccerMenu::draw(const Common::Soccer::State &t_state)
{
    ImGui::Text("random_param: %f", t_state.random_param);
}
} // namespace Tyr::Gui
