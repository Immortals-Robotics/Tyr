#include "ai.h"

namespace Tyr::Soccer
{
void Ai::internalProcessData()
{
    if (Common::worldState().ball.seen_state != Common::SeenState::CompletelyOut)
        ballHist.push_back(Common::worldState().ball);
    if (ballHist.size() > maxBallHist)
        ballHist.pop_front();
    calculateBallTrajectory();
    debugDraw = false;

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        bool halt_this_robot_for_now = false;

        if (Common::refereeState().stop())
        {
            if (OwnRobot[i].state().out_for_substitute)
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
                            OwnRobot[i].setVisionId(j);
                            break;
                        }
                    }
                }
            }
        }

        OwnRobot[i].shoot    = 0;
        OwnRobot[i].dribbler = 0;
        OwnRobot[i].shoot    = 0;
        OwnRobot[i].chip     = 0;
        navigated[i]         = false;
    }

    if (Common::setting().our_side == Common::TeamSide::Right)
        side = 1;
    else
        side = -1;
}
} // namespace Tyr::Soccer
