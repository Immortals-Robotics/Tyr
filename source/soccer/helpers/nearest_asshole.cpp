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
        if (OppRobot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if ((std::fabs(OppRobot[i].position.x) > field_width) || (std::fabs(OppRobot[i].position.y) > field_height))
            continue;
        if ((!acceptNearBall) && ((ball.position.distanceTo(OppRobot[i].position) < 500)))
        {
            continue;
        }
        if ((pos.distanceTo(OppRobot[i].position) < mdis))
        {
            mdis  = pos.distanceTo(OppRobot[i].position);
            index = i;
        }
    }

    return index;
}
} // namespace Tyr::Soccer
