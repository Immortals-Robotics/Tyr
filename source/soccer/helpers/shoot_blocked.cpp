#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::shootBlocked(const Common::Vec2 t_init_pos, const Common::Vec2 t_target_pos, const float t_max_shoot_blocker_dis, const float t_shoot_blocker_r)
{
    bool oppGoalOpen = true;

    Common::LineSegment shootLine = Common::LineSegment(t_target_pos, t_init_pos);
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if (m_world_state.opp_robot[i].position.distanceTo(t_init_pos) > t_max_shoot_blocker_dis)
            continue;

        if (shootLine.distanceTo(m_world_state.opp_robot[i].position) < t_shoot_blocker_r)
        {
            oppGoalOpen = false;
            break;
        }
    }

    return !oppGoalOpen;
}
} // namespace Tyr::Soccer
