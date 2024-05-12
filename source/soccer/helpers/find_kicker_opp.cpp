#include "../ai.h"

namespace Tyr::Soccer
{
int Ai::findKickerOpp(int mask, float max_dis)
{
    float mdis  = max_dis;
    int   index = -1;
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (i == mask)
            continue;
        if (m_state.opp_robot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if (m_state.ball.position.distanceTo(m_state.opp_robot[i].position) < mdis)
        {
            mdis  = m_state.ball.position.distanceTo(m_state.opp_robot[i].position);
            index = i;
        }
    }
    return index;
}
} // namespace Tyr::Soccer
