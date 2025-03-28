#include "../ai.h"

#include "../tactics/gk.h"
#include "../tactics/def.h"

namespace Tyr::Soccer
{
void Ai::theirPlaceBall()
{
    calcIsDefending();

    m_assignments.clear();
    createGkAssignment();
    createDefAssignments();
    createMidAssignments();
    createAttackAssignment();
    assignRoles();

    GkTactic{}.execute(m_own_robot[m_gk]);

    DefTactic{1}.execute(m_own_robot[m_def1]);
    DefTactic{2}.execute(m_own_robot[m_def2]);

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        m_own_robot[*mid].face(m_world_state.ball.position);
        m_own_robot[*mid].navigate(m_sorted_zones[zone_idx]->best_pos, VelocityProfile::aroom(), NavigationFlags::BallObstacle);
        ++zone_idx;
    }

    m_own_robot[m_attack].face(m_world_state.ball.position);
    m_own_robot[m_attack].navigate(
             m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(Field::ownGoal()), 1090),
             VelocityProfile::aroom());
}
} // namespace Tyr::Soccer
