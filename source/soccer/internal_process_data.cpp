#include "ai.h"

namespace Tyr::Soccer
{
void Ai::internalProcessData()
{
    m_ball_trajectory.update(m_world_state.ball);
    m_ball_trajectory.calculate();

    for (Robot &robot : m_own_robot)
    {
        robot.reset();
    }

    if (m_ref_state.our_side == Common::TeamSide::Right)
        m_side = 1;
    else
        m_side = -1;
}
} // namespace Tyr::Soccer
