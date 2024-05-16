#include "../ai.h"

namespace Tyr::Soccer
{
int Ai::findNearestOpp(Common::Vec2 pos, int mask, bool acceptNearBall)
{
    float mdis  = std::numeric_limits<float>::max();
    int   index = -1;
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (i == mask)
            continue;
        if (m_world_state.opp_robot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if (isOut(m_world_state.opp_robot[i].position))
            continue;
        if ((!acceptNearBall) && ((m_world_state.ball.position.distanceTo(m_world_state.opp_robot[i].position) < 500)))
        {
            continue;
        }
        if ((pos.distanceTo(m_world_state.opp_robot[i].position) < mdis))
        {
            mdis  = pos.distanceTo(m_world_state.opp_robot[i].position);
            index = i;
        }
    }

    return index;
}
} // namespace Tyr::Soccer
