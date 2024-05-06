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
        if (Common::worldState().opp_robot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if ((std::fabs(Common::worldState().opp_robot[i].position.x) > Common::field().width) || (std::fabs(Common::worldState().opp_robot[i].position.y) > Common::field().height))
            continue;
        if ((!acceptNearBall) && ((Common::worldState().ball.position.distanceTo(Common::worldState().opp_robot[i].position) < 500)))
        {
            continue;
        }
        if ((pos.distanceTo(Common::worldState().opp_robot[i].position) < mdis))
        {
            mdis  = pos.distanceTo(Common::worldState().opp_robot[i].position);
            index = i;
        }
    }

    return index;
}
} // namespace Tyr::Soccer
