#include "../ai.h"

namespace Tyr::Soccer
{
int Ai::findNearestAsshole(Common::Vec2 pos, int mask, bool acceptNearBall)
{
    float mdis  = 7000;
    int   index = -1;
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (i == mask)
            continue;
        if (OppRobot[i].seenState == Common::CompletelyOut)
            continue;
        if ((std::fabs(OppRobot[i].Position.x) > field_width) || (std::fabs(OppRobot[i].Position.y) > field_height))
            continue;
        if ((!acceptNearBall) && ((ball.Position.distanceTo(OppRobot[i].Position) < 500)))
        {
            continue;
        }
        if ((pos.distanceTo(OppRobot[i].Position) < mdis))
        {
            mdis  = pos.distanceTo(OppRobot[i].Position);
            index = i;
        }
    }

    return index;
}
} // namespace Tyr::Soccer
