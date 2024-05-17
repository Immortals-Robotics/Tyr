#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::goalBlocked(Common::Vec2 t_init_pos, float t_max_shoot_blocker_dis, float t_shoot_blocker_r)
{
    bool oppGoalOpen = true;

    Common::Line ballGoalLine = Common::Line::fromTwoPoints(oppGoal(), t_init_pos);
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if ((std::fabs(m_world_state.opp_robot[i].position.x) > Common::field().width) ||
            (std::fabs(m_world_state.opp_robot[i].position.y) > Common::field().height))
            continue;
        if (m_world_state.opp_robot[i].position.distanceTo(t_init_pos) > t_max_shoot_blocker_dis)
            continue;

        if (ballGoalLine.distanceTo(m_world_state.opp_robot[i].position) < t_shoot_blocker_r)
        {
            oppGoalOpen = false;
            break;
        }
    }

    return !oppGoalOpen;
}
} // namespace Tyr::Soccer
