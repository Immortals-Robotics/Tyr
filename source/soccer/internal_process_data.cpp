#include "ai.h"

namespace Tyr::Soccer
{
void Ai::internalProcessData()
{
    if (Common::worldState().ball.seen_state != Common::SeenState::CompletelyOut)
        ballHist.push_back(Common::worldState().ball);
    if (ballHist.size() > maxBallHist)
        ballHist.pop_front();
    // debugDraw = true;
    calculateBallTrajectory();
    debugDraw = false;

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        bool halt_this_robot_for_now = false;
        OwnRobot[i].State            = Common::worldState().own_robot[OwnRobot[i].vision_id];

        if (Common::refereeState().stop())
        {
            if (OwnRobot[i].State.out_for_substitute)
            {
                for (int j = 0; j < Common::Setting::kMaxRobots; j++)
                {
                    if ((Common::worldState().own_robot[j].seen_state == Common::SeenState::Seen) &&
                        (std::fabs(Common::worldState().own_robot[j].position.x) < Common::worldState().field.width) &&
                        (std::fabs(Common::worldState().own_robot[j].position.y) < Common::worldState().field.height))
                    {
                        bool suitable = true;
                        for (int k = 0; k < Common::Setting::kMaxOnFieldTeamRobots; k++)
                        {
                            if (OwnRobot[k].vision_id == j)
                            {
                                suitable = false;
                                break;
                            }
                        }
                        if (suitable)
                        {
                            OwnRobot[i].set_vision_id(j);
                            break;
                        }
                    }
                }
            }
        }
        // std::cout << OwnRobot[i].vision_id << "	";

        OwnRobot[i].set_serial_id(OwnRobot[i].vision_id);
        // OwnRobot[i].oldRobot = true;
        // if ( ( i != gk ) && ( i != def1 ) )
        // OwnRobot[i].oldRobot = false;

        OwnRobot[i].shoot    = 0;
        OwnRobot[i].dribbler = 0;
        OwnRobot[i].shoot    = 0;
        OwnRobot[i].chip     = 0;
        OwnRobot[i].Break    = 0;
        navigated[i]         = false;

        // if ((OwnRobot[i].vision_id==7)||(OwnRobot[i].vision_id==4)||(OwnRobot[i].vision_id==0)) {
        //	OwnRobot[i].oldRobot = true;
        // }
        // if (OwnRobot[i].vision_id==10) {
        //	OwnRobot[i].oldRobot = true;
        // }
        // OwnRobot[def].oldRobot = true;
        // if(i==passgir)
        //	OwnRobot[i].oldRobot = true;
    }
    // std::cout << std::endl;

    if (Common::setting().our_side == Common::TeamSide::Right)
        side = 1;
    else
        side = -1;
}
} // namespace Tyr::Soccer
