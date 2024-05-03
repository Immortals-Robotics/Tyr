#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::wantThisRobot(int robot_num)
{
    requiredRobots[robot_num] = true;
}

bool Ai::positionRobots(bool avoid_GK, bool avoid_DEF)
{
    //    if(avoid_GK)
    //        requiredRobots[avoid_GK] = true;
    //    if(avoid_DEF)
    //        requiredRobots[avoid_DEF] = true;

    for (int j = 0; j < Common::Setting::kMaxOnFieldTeamRobots; j++)
    {
        if (requiredRobots[*stm2AInum[j]] &&
            OwnRobot[*stm2AInum[j]].state().seen_state == Common::SeenState::CompletelyOut)
        { // Find the robot that needs to be replaced

            int i;
            for (i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
            {
                if (!requiredRobots[*stm2AInum[i]] &&
                    OwnRobot[*stm2AInum[i]].state().seen_state != Common::SeenState::CompletelyOut &&
                    gk != *stm2AInum[i] && def != *stm2AInum[i])
                {
                    std::swap(*stm2AInum[i], *stm2AInum[j]);
                    requiredRobots[*stm2AInum[j]] = true;
                    requiredRobots[*stm2AInum[i]] = false;

                    j = -1;
                    break;
                }
            }
            if (i == Common::Setting::kMaxOnFieldTeamRobots)
            {
                for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
                {
                    requiredRobots[i] = false;
                }
                Common::logError("FAILED SWAP");
                return false;
            }
        }
    }
    Common::logDebug("DONE SWAP");

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        requiredRobots[i] = false;
    }
    return true;
}
} // namespace Tyr::Soccer
