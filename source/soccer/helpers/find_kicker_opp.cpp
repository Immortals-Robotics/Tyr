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
        if (OppRobot[i].seenState == Common::CompletelyOut)
            continue;
        if (ball.Position.distanceTo(OppRobot[i].Position) < mdis)
        {
            mdis  = ball.Position.distanceTo(OppRobot[i].Position);
            index = i;
        }
    }
    return index;
}
} // namespace Tyr::Soccer
