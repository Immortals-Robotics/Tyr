#include "ai.h"

namespace Tyr::Soccer
{
void Ai::process()
{
    static int PRCS_CNT = 0;

    internalPrepare();
    internalProcessData();

    if (lastReferee != m_ref_state.get())
    {
        timer.start();
        lastReferee = m_ref_state.get();
        randomParam = m_random.get(0.0f, 1.0f);
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
            currentPlay = &Ai::Stop_def;
        }
        else
        {
            currentPlay = &Ai::Stop;
        }
    }
    else if (m_ref_state.gameOn())
    {
        currentPlay = &Ai::NewNormalPlay;
    }
    else if (m_ref_state.ourKickoff())
    {
        currentPlay = &Ai::kickoff_us_chip;
    }
    else if (m_ref_state.ourDirectKick() || m_ref_state.ourIndirectKick())
    {
        if (target_str != -1)
        {
            currentPlay = &Ai::strategy;
        }
        else
        {
            currentPlay = &Ai::throwin_chip_shoot;
        }
    }
    else if (m_ref_state.ourPenaltyKick())
    {
        currentPlay = &Ai::penalty_us_shootout;
    }
    else if (m_ref_state.ourPlaceBall())
    {
        currentPlay = &Ai::our_place_ball;
    }
    else if (m_ref_state.theirFreeKick())
    {
        currentPlay = &Ai::corner_their_global;
    }
    else if (m_ref_state.theirKickoff())
    {
        currentPlay = &Ai::kickoff_their_one_wall;
    }
    else if (m_ref_state.theirPenaltyKick())
    {
        currentPlay = &Ai::penalty_their_simple;
    }
    else if (m_ref_state.theirPlaceBall())
    {
        currentPlay = &Ai::their_place_ball;
    }
    else if (!m_ref_state.canMove())
    {
        currentPlay = &Ai::HaltAll;
    }
    else
    {
        Common::logWarning("Unhandled ref state: {}", (int) m_ref_state.get());
        currentPlay = &Ai::Stop;
    }

    if (m_ref_state.theirRestart())
    {
        oppRestarted = true;
    }

    (this->*currentPlay)();

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OwnRobot[i].state().seen_state == Common::SeenState::CompletelyOut)
        {
            Halt(i);
        }
        else if (!navigated[i])
        {
            Common::logWarning("Own robot {} was not navigated", OwnRobot[i].state().vision_id);
            Halt(i);
        }
    }
}
} // namespace Tyr::Soccer
