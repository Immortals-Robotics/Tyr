#include "ai.h"

namespace Tyr::Soccer
{
void Ai::process()
{
    static int PRCS_CNT = 0;

    internalProcessData();

    if (m_last_referee != m_ref_state.get())
    {
        m_timer.start();
        m_last_referee = m_ref_state.get();
        m_random_param = m_random.get(0.0f, 1.0f);
        m_target_str   = strategyWeight();
        m_func_state = 0;
        m_func_count = 0;
    }

    if (m_ref_state.stop())
    {
        m_func_state = 0;

        m_opp_restarted = false;
        if (m_side * m_world_state.ball.position.x > Common::field().width * 0.7f)
        {
            m_current_play = &Ai::stopDef;
        }
        else
        {
            m_current_play = &Ai::stop;
        }
    }
    else if (m_ref_state.gameOn())
    {
        m_current_play = &Ai::newNormalPlay;
    }
    else if (m_ref_state.ourKickoff())
    {
        m_current_play = &Ai::kickoffUsChip;
    }
    else if (m_ref_state.ourDirectKick() || m_ref_state.ourIndirectKick())
    {
        if (m_target_str != -1)
        {
            m_current_play = &Ai::strategy;
        }
        else
        {
            m_current_play = &Ai::throwinChipShoot;
        }
    }
    else if (m_ref_state.ourPenaltyKick())
    {
        m_current_play = &Ai::penaltyUsShootout;
    }
    else if (m_ref_state.ourPlaceBall())
    {
        m_current_play = &Ai::ourPlaceBall;
    }
    else if (m_ref_state.theirFreeKick())
    {
        m_current_play = &Ai::cornerTheirGlobal;
    }
    else if (m_ref_state.theirKickoff())
    {
        m_current_play = &Ai::kickoffTheirOneWall;
    }
    else if (m_ref_state.theirPenaltyKick())
    {
        m_current_play = &Ai::penaltyTheirSimple;
    }
    else if (m_ref_state.theirPlaceBall())
    {
        m_current_play = &Ai::theirPlaceBall;
    }
    else if (!m_ref_state.canMove())
    {
        m_current_play = &Ai::haltAll;
    }
    else
    {
        Common::logWarning("Unhandled ref state: {}", static_cast<int>(m_ref_state.get()));
        m_current_play = &Ai::stop;
    }

    if (m_ref_state.theirRestart())
    {
        m_opp_restarted = true;
    }

    (this->*m_current_play)();

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_own_robot[i].state().seen_state == Common::SeenState::CompletelyOut)
        {
            halt(i);
        }
        else if (!m_own_robot[i].navigated() && !m_own_robot[i].halted())
        {
            Common::logWarning("Own robot {} was neither navigated nor halted", m_own_robot[i].state().vision_id);
            halt(i);
        }
    }
}
} // namespace Tyr::Soccer
