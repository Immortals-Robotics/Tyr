#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::goalBlocked(const Common::Vec2 t_init_pos, const float t_max_shoot_blocker_dis, const float t_shoot_blocker_r)
{
    bool oppGoalOpen = true;

    Common::Line ballGoalLine = Common::Line::fromTwoPoints(Field::oppGoal(), t_init_pos);
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
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
