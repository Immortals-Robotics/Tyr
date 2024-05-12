#include "ai.h"

namespace Tyr::Soccer
{
void Ai::process()
{
    static int PRCS_CNT = 0;

    internalPrepare();
    internalProcessData();

    Common::debug().draw(m_world_state.ball.position, Common::Color::red());
    //	Common::debug().draw(m_world_state.ball.position,Common::Vec2(m_world_state.ball.velocity.x,m_world_state.ball.velocity.y)
    //+ m_world_state.ball.position,
    // Black);

    if (lastReferee != m_ref_state.get())
    {
        timer.start();
        lastReferee = m_ref_state.get();
        randomParam = m_random.get();
        target_str  = strategy_weight();
        FUNC_state  = 0;
        FUNC_CNT    = 0;
    }

    if (m_ref_state.stop())
    {
        FUNC_state = 0;

        oppRestarted = false;
        if (side * m_world_state.ball.position.x > Common::field().width * 0.7f)
        {
            currentPlay = "Stop_def";
        }
        else
        {
            currentPlay = "Stop";
        }
    }
    else if (m_ref_state.gameOn())
    {
        currentPlay = "NewNormalPlay";
    }
    else if (m_ref_state.ourKickoff())
    {
        currentPlay = "kickoff_us_chip";
    }
    else if ((m_ref_state.ourDirectKick()) || (m_ref_state.ourIndirectKick()))
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
    else if (m_ref_state.ourPenaltyKick())
    {
        currentPlay = "penalty_us_shootout";
    }
    else if (m_ref_state.ourPlaceBall())
    {
        //            m_ref_state.place_ball_target.X = -2500;
        //            m_ref_state.place_ball_target.Y = -1500;
        currentPlay = "our_place_ball_shoot";
        currentPlay = "our_place_ball_shoot_V2"; // COMMENT this if it's not working...
        //			currentPlay = "our_place_ball_shoot_taki";
    }
    else if (m_ref_state.theirFreeKick())
    {
        currentPlay = "corner_their_global";
    }
    else if (m_ref_state.theirKickoff())
    {
        currentPlay = "kickoff_their_one_wall";
    }
    else if (m_ref_state.theirPenaltyKick())
    {
        currentPlay = "penalty_their_simple";
    }
    else if (m_ref_state.theirPlaceBall())
    {
        currentPlay = "their_place_ball";
    }
    else if (m_ref_state.get() == Common::RefereeState::STATE_HALTED)
    {
        currentPlay = "HaltAll";
    }
    else
    {
        currentPlay = "Stop";
    }

    if (m_ref_state.theirRestart())
    {
        oppRestarted = true;
    }

    if (AIPlayBook.find(currentPlay) != AIPlayBook.end())
        (this->*AIPlayBook[currentPlay])();
    else
    {
        HaltAll();
        Common::logWarning("Can't find play \"{}\" in playbook", currentPlay);
    }

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        if ((OwnRobot[i].state().seen_state == Common::SeenState::CompletelyOut) || (!navigated[i]))
        {
            Halt(i);
        }
    }
}
} // namespace Tyr::Soccer
