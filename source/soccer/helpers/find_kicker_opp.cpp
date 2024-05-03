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
        if (Common::worldState().opp_robot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if (Common::worldState().ball.position.distanceTo(Common::worldState().opp_robot[i].position) < mdis)
        {
            mdis  = Common::worldState().ball.position.distanceTo(Common::worldState().opp_robot[i].position);
            index = i;
        }
    }
    return index;
}
} // namespace Tyr::Soccer
