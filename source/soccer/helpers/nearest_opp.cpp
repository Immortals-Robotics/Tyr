#include "../ai.h"

namespace Tyr::Soccer
{
std::optional<Common::RobotState> Ai::findNearestOpp(const Common::Vec2 t_pos, const int t_mask,
                                                     const bool t_accept_near_ball)
{
    float mdis  = std::numeric_limits<float>::max();
    int   index = -1;
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        if (i == t_mask)
            continue;
        if (m_world_state.opp_robot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if (isOut(m_world_state.opp_robot[i].position))
            continue;
        if ((!t_accept_near_ball) &&
            ((m_world_state.ball.position.distanceTo(m_world_state.opp_robot[i].position) < 500)))
        {
            continue;
        }
        if ((t_pos.distanceTo(m_world_state.opp_robot[i].position) < mdis))
        {
            mdis  = t_pos.distanceTo(m_world_state.opp_robot[i].position);
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
