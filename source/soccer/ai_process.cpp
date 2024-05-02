#include "ai.h"

namespace Tyr::Soccer
{
void Ai::Process(Common::WorldState *worldState)
{
    static int PRCS_CNT = 0;

    debugDraw = false;

    internalProcessData(worldState);

    debugDraw = true;
    Common::debug().drawCircle(ball.position, 40, "", Common::Red);
    //	Common::debug().drawLineSegment(ball.position,Common::Vec2(ball.velocity.x,ball.velocity.y) + ball.position,
    //Black);

    debugDraw = false;

    if (REF_playState)
    {
        if (lastReferee != REF_playState->get())
        {
            timer.start();
            lastReferee = REF_playState->get();
            randomParam = random.get();
            target_str  = strategy_weight();
            FUNC_state  = 0;
            FUNC_CNT    = 0;
        }

        if (REF_playState->stop())
        {
            FUNC_state = 0;

            oppRestarted = false;
            if (side * ball.position.x > field_width * 0.7f)
            {
                currentPlay = "Stop_def";
            }
            else
            {
                currentPlay = "Stop";
            }
        }
        else if (REF_playState->gameOn())
        {
            currentPlay = "NewNormalPlay";
        }
        else if (REF_playState->ourKickoff())
        {
            currentPlay = "kickoff_us_chip";

            currentPlayParam = static_cast<uint32_t>(refereeState->state.canKickBall());
        }
        else if ((REF_playState->ourDirectKick()) || (REF_playState->ourIndirectKick()))
        {
            if (target_str != -1)
            {
                currentPlay      = "strategy_maker";
                currentPlayParam = static_cast<uint32_t>(target_str);
            }
            else
            {
                currentPlay = "throwin_chip_shoot";
            }
            std::cout << currentPlay << std::endl;
        }
        else if (REF_playState->ourPenaltyKick())
        {
            currentPlay      = "penalty_us_shootout";
            currentPlayParam = static_cast<uint32_t>(refereeState->state.canKickBall());
            //			std::cout << "IN_PENALTY..."<<worldState ->refereeState -> state.canKickBall()<<std::endl;
        }
        else if (REF_playState->ourPlaceBall())
        {
            //            targetBallPlacement->X = -2500;
            //            targetBallPlacement->Y = -1500;
            currentPlay = "our_place_ball_shoot";
            currentPlay = "our_place_ball_shoot_V2"; // COMMENT this if it's not working...
            //			currentPlay = "our_place_ball_shoot_taki";
        }
        else if (REF_playState->theirFreeKick())
        {
            currentPlay = "corner_their_global";
        }
        else if (REF_playState->theirKickoff())
        {
            currentPlay = "kickoff_their_one_wall";
        }
        else if (REF_playState->theirPenaltyKick())
        {
            currentPlay = "penalty_their_simple";
        }
        else if (REF_playState->theirPlaceBall())
        {
            currentPlay = "their_place_ball";
        }
        else if (REF_playState->get() == Common::GameState::STATE_HALTED)
        {
            currentPlay = "HaltAll";
        }
        else
        {
            currentPlay = "Stop";
        }

        if (REF_playState->theirRestart())
        {
            oppRestarted = true;
        }
    }
    else
    {
        currentPlay = "Stop";
        // currentPlay = "HaltAll";
    }
    //	currentPlay = "my_test"; // TODO comment this in the game
    //	currentPlay = "penalty_our_Shoot_Out";
    //	currentPlay = "NewNormalPlay";

    if (AIPlayBook.find(currentPlay) != AIPlayBook.end())
        (this->*AIPlayBook[currentPlay])();
    else
    {
        HaltAll();
        // std::cout<<"Oh NO!!!"<<std::endl;
    }

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        if ((OwnRobot[i].State.seen_state == Common::SeenState::CompletelyOut) || (!navigated[i]))
        {
            Halt(i);
        }
    }

    internalFinalize(worldState);
}
} // namespace Tyr::Soccer
