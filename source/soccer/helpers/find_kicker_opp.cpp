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
        if (OppRobot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if (ball.position.distanceTo(OppRobot[i].position) < mdis)
        {
            mdis  = ball.position.distanceTo(OppRobot[i].position);
            index = i;
        }
    }
    return index;
}
} // namespace Tyr::Soccer
