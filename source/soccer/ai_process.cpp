#include "ai.h"

namespace Tyr::Soccer
{
void Ai::process()
{
    internalProcessData();

    if (m_last_referee != m_ref_state.state)
    {
        m_timer.start();
        m_last_referee = m_ref_state.state;
        m_random_param = m_random.get(0.0f, 1.0f);
        m_target_str   = strategyWeight();
        m_our_ball_placement_state = OurBallPlacementState::Idle;
    }

    if (m_ref_state.stop())
    {
        m_opp_restarted = false;
        m_current_play  = &Ai::stop;
    }
    else if (m_ref_state.running())
    {
        m_current_play = &Ai::newNormalPlay;
    }
    else if (m_ref_state.ourKickoff())
    {
        m_current_play = &Ai::kickoffUsChip;
    }
    else if (m_ref_state.ourFreeKick())
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
    else if (m_ref_state.ourBallPlacement())
    {
        m_current_play = &Ai::placeBall;
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
    else if (m_ref_state.theirBallPlacement())
    {
        m_current_play = &Ai::theirPlaceBall;
    }
    else if (m_ref_state.halt() || m_ref_state.state == Common::Referee::GameState::None)
    {
        m_current_play = &Ai::haltAll;
    }
    else
    {
        Common::logWarning("Unhandled ref state: {}", m_ref_state);
        m_current_play = &Ai::stop;
    }

    if (m_ref_state.theirRestart())
    {
        m_opp_restarted = true;
    }

    (this->*m_current_play)();

    for (Robot& robot : m_own_robot)
    {
        if (robot.state().seen_state == Common::SeenState::CompletelyOut)
        {
            halt(robot);
        }
        else if (!robot.navigated() && !robot.halted())
        {
            Common::logWarning("Own robot {} was neither navigated nor halted", robot.state().vision_id);
            halt(robot);
        }
    }
}
} // namespace Tyr::Soccer
