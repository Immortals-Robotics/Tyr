#include "ai.h"

namespace Tyr::Soccer
{
void Ai::internalProcessData(Common::WorldState *worldState)
{
    this->oppGK = refereeState->opp_gk;

    this->ball = worldState->ball;
    if (ball.seen_state != Common::SeenState::CompletelyOut)
        this->ballHist.push_back(this->ball);
    if (this->ballHist.size() > maxBallHist)
        this->ballHist.pop_front();
    // debugDraw = true;
    CalculateBallTrajectory();
    debugDraw = false;

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        bool halt_this_robot_for_now = false;
        this->OwnRobot[i].State      = worldState->own_robot[OwnRobot[i].vision_id];

        if (refereeState->get() == Common::RefereeState::STATE_GAME_OFF)
        {
            if (OwnRobot[i].State.out_for_substitute)
            {
                for (int j = 0; j < Common::Setting::kMaxRobots; j++)
                {
                    if ((worldState->own_robot[j].seen_state == Common::SeenState::Seen) &&
                        (std::fabs(worldState->own_robot[j].position.x) < field_width) &&
                        (std::fabs(worldState->own_robot[j].position.y) < field_height))
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

        this->OwnRobot[i].set_serial_id(OwnRobot[i].vision_id);
        // this->OwnRobot[i].oldRobot = true;
        // if ( ( i != gk ) && ( i != def1 ) )
        // this->OwnRobot[i].oldRobot = false;

        this->OwnRobot[i].shoot    = 0;
        this->OwnRobot[i].dribbler = 0;
        this->OwnRobot[i].shoot    = 0;
        this->OwnRobot[i].chip     = 0;
        this->OwnRobot[i].Break    = 0;
        this->navigated[i]         = false;

        // if ((OwnRobot[i].vision_id==7)||(OwnRobot[i].vision_id==4)||(OwnRobot[i].vision_id==0)) {
        //	this->OwnRobot[i].oldRobot = true;
        // }
        // if (OwnRobot[i].vision_id==10) {
        //	this->OwnRobot[i].oldRobot = true;
        // }
        // this->OwnRobot[def].oldRobot = true;
        // if(i==passgir)
        //	this->OwnRobot[i].oldRobot = true;
    }
    // std::cout << std::endl;

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
        this->OppRobot[i] = worldState->opp_robot[i];

    if (Common::setting().our_side == Common::TeamSide::Right)
        this->side = 1;
    else
        this->side = -1;
}
} // namespace Tyr::Soccer
