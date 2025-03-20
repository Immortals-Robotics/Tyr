#include "../ai.h"

namespace Tyr::Soccer
{
std::optional<Common::RobotState> Ai::findKickerOpp(const int t_mask, const float t_max_dis)
{
    float mdis  = t_max_dis;
    int   index = -1;
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        if (i == t_mask)
            continue;
        if (m_world_state.opp_robot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if (m_world_state.ball.position.distanceTo(m_world_state.opp_robot[i].position) < mdis)
        {
            mdis  = m_world_state.ball.position.distanceTo(m_world_state.opp_robot[i].position);
            index = i;
        }
    }

    if (index == -1)
    {
        return std::nullopt;
    }

    return m_world_state.opp_robot[index];
}
} // namespace Tyr::Soccer
