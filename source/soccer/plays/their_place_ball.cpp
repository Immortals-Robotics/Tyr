#include "../ai.h"

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

    gkHi(m_gk);
    defHi(m_def1, m_def2, nullptr);

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        m_own_robot[*mid].face(m_world_state.ball.position);
        m_own_robot[*mid].navigate(m_sorted_zones[zone_idx]->best_pos, VelocityProfile::aroom(), NavigationFlags::ForceBallObstacle);
        ++zone_idx;
    }

    m_own_robot[m_attack].face(m_world_state.ball.position);
    m_own_robot[m_attack].navigate(
             m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 1090),
             VelocityProfile::aroom());
}
} // namespace Tyr::Soccer
