#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::want_this_robot(int robot_num)
{
    requiredRobots[robot_num] = true;
}

bool Ai::position_robots(bool avoid_GK, bool avoid_DEF)
{
    //    if(avoid_GK)
    //        requiredRobots[avoid_GK] = true;
    //    if(avoid_DEF)
    //        requiredRobots[avoid_DEF] = true;

    for (int j = 0; j < Common::Setting::kMaxOnFieldTeamRobots; j++)
    {
        //        std::cout<<"j is: "<<j<<std::endl;
        if (requiredRobots[*stm2AInum[j]] && OwnRobot[*stm2AInum[j]].State.seen_state == Common::SeenState::CompletelyOut)
        { // Find the robot that needs to be replaced

            int i;
            for (i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
            {
                if (!requiredRobots[*stm2AInum[i]] &&
                    OwnRobot[*stm2AInum[i]].State.seen_state != Common::SeenState::CompletelyOut && gk != *stm2AInum[i] &&
                    def != *stm2AInum[i])
                {
                    //                    std::cout<<"BEF_SWAP: "<<requiredRobots[*stm2AInum[j]]<<
                    //                    (OwnRobot[*stm2AInum[j]].State.seen_state == Common::SeenState::CompletelyOut)
                    //                    <<std::endl;
                    std::swap(*stm2AInum[i], *stm2AInum[j]);
                    //                    int tempint = *stm2AInum[i];
                    //                    *stm2AInum[i] = *stm2AInum[j];
                    //                    *stm2AInum[j] = tempint;
                    requiredRobots[*stm2AInum[j]] = true;
                    requiredRobots[*stm2AInum[i]] = false;
                    //                    std::cout<<"DATA_1: "<<requiredRobots[*stm2AInum[3]]<<
                    //                    (OwnRobot[*stm2AInum[3]].State.seen_state == Common::SeenState::CompletelyOut)
                    //                    <<std::endl; std::cout<<"DATA_2: "<<requiredRobots[*stm2AInum[4]]<<
                    //                    (OwnRobot[*stm2AInum[4]].State.seen_state == Common::SeenState::CompletelyOut)
                    //                    <<std::endl;

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
                std::cout << "FAILED SWAP" << std::endl;
                return false;
            }
        }
    }
    std::cout << "DONE SWAP" << std::endl;

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        requiredRobots[i] = false;
    }
    return true;
}
} // namespace Tyr::Soccer
