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
                for (const Common::RobotState &robot : m_world_state.own_robot)
                {
                    if (robot.seen_state == Common::SeenState::Seen && !isOut(robot.position, 20.0f))
                    {
                        const bool taken =
                            std::any_of(ids.begin(), ids.end(), [robot](int *id) { return *id == robot.vision_id; });

                        if (!taken)
                        {
                            *id = robot.vision_id;
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
