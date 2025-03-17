#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::calcIsDefending()
{
    if (m_is_defending)
    {
        if (m_side * m_world_state.ball.position.x < -800)
        {
            m_is_defending = false;
        }
    }
    else if (m_side * m_world_state.ball.position.x > -300)
    {
        m_is_defending = true;
    }

    if (m_opp_restarted && m_timer.time().seconds() < 2.0)
    {
        m_is_defending = true;
    }
}
} // namespace Tyr::Soccer
