#include "ai.h"

namespace Tyr::Soccer
{
void Ai::internalProcessData()
{
    m_ball_trajectory.update(m_world_state.ball);
    m_ball_trajectory.calculate();

    if (m_ref_state.stop())
    {
        for (int *id : ids)
        {
            if (id == &m_gk)
            {
                const int new_gk_id = m_ref_state.ourInfo().gk_id;

                if (new_gk_id != *id)
                {
                    const auto current_assignemt =
                        std::find_if(ids.begin(), ids.end(), [new_gk_id](int *id) { return *id == new_gk_id; });

                    if (current_assignemt != ids.end())
                    {
                        **current_assignemt = *id;
                    }

                    *id = new_gk_id;
                }

                continue;
            }

            if (m_own_robot[*id].state().out_for_substitute)
            {
                for (int j = 0; j < Common::Config::Common::kMaxRobots; j++)
                {
                    if ((m_world_state.own_robot[j].seen_state == Common::SeenState::Seen) &&
                        std::fabs(m_world_state.own_robot[j].position.x) < Common::field().width &&
                        std::fabs(m_world_state.own_robot[j].position.y) < Common::field().height)
                    {
                        const bool taken = std::any_of(ids.begin(), ids.end(), [j](int *id) { return *id == j; });

                        if (!taken)
                        {
                            *id = j;
                            break;
                        }
                    }
                }
            }
        }
    }

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
