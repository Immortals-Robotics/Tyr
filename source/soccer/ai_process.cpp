#include "ai.h"

namespace Tyr::Soccer
{
void Ai::Process()
{
    static int PRCS_CNT = 0;

    debugDraw = false;

    internalProcessData();

    debugDraw = true;
    Common::debug().draw(Common::worldState().ball.position, Common::Color::red());
    //	Common::debug().draw(Common::worldState().ball.position,Common::Vec2(Common::worldState().ball.velocity.x,Common::worldState().ball.velocity.y)
    //+ Common::worldState().ball.position,
    // Black);

    debugDraw = false;

    if (lastReferee != Common::refereeState().get())
    {
        timer.start();
        lastReferee = Common::refereeState().get();
        randomParam = m_random.get();
        target_str  = strategy_weight();
        FUNC_state  = 0;
        FUNC_CNT    = 0;
    }

    if (Common::refereeState().stop())
    {
        FUNC_state = 0;

        oppRestarted = false;
        if (side * Common::worldState().ball.position.x > Common::worldState().field.width * 0.7f)
        {
            currentPlay = "Stop_def";
        }
        else
        {
            currentPlay = "Stop";
        }
    }
    else if (Common::refereeState().gameOn())
    {
        currentPlay = "NewNormalPlay";
    }
    else if (Common::refereeState().ourKickoff())
    {
        currentPlay = "kickoff_us_chip";
    }
    else if ((Common::refereeState().ourDirectKick()) || (Common::refereeState().ourIndirectKick()))
    {
        if (target_str != -1)
        {
            currentPlay = "strategy_maker";
        }
        else
        {
            currentPlay = "throwin_chip_shoot";
        }
        Common::logDebug("free-kick play: {}", currentPlay);
    }
    else if (Common::refereeState().ourPenaltyKick())
    {
        currentPlay = "penalty_us_shootout";
    }
    else if (Common::refereeState().ourPlaceBall())
    {
        //            Common::refereeState().place_ball_target.X = -2500;
        //            Common::refereeState().place_ball_target.Y = -1500;
        currentPlay = "our_place_ball_shoot";
        currentPlay = "our_place_ball_shoot_V2"; // COMMENT this if it's not working...
        //			currentPlay = "our_place_ball_shoot_taki";
    }
    else if (Common::refereeState().theirFreeKick())
    {
        currentPlay = "corner_their_global";
    }
    else if (Common::refereeState().theirKickoff())
    {
        currentPlay = "kickoff_their_one_wall";
    }
    else if (Common::refereeState().theirPenaltyKick())
    {
        currentPlay = "penalty_their_simple";
    }
    else if (Common::refereeState().theirPlaceBall())
    {
        currentPlay = "their_place_ball";
    }
    else if (Common::refereeState().get() == Common::RefereeState::STATE_HALTED)
    {
        currentPlay = "HaltAll";
    }
    else
    {
        currentPlay = "Stop";
    }

    if (Common::refereeState().theirRestart())
    {
        oppRestarted = true;
    }

    if (AIPlayBook.find(currentPlay) != AIPlayBook.end())
        (this->*AIPlayBook[currentPlay])();
    else
    {
        HaltAll();
        Common::logWarning("Oh NO!!!");
    }

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        if ((OwnRobot[i].state().seen_state == Common::SeenState::CompletelyOut) || (!navigated[i]))
        {
            Halt(i);
        }
    }

    internalFinalize();
}
} // namespace Tyr::Soccer
