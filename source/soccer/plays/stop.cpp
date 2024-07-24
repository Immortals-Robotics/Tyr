#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::stop()
{
    calcIsDefending();
    m_is_defending = m_is_defending && Common::config().soccer.mark_in_stop;

    m_assignments.clear();
    createGkAssignment();
    createDefAssignments();
    createMidAssignments();
    createAttackAssignment();
    assignRoles();

    gkHi(m_gk);
    defHi(m_def1, m_def2, nullptr);

    int zone_idx = 0;
    for (int mid_idx = 0; mid_idx < m_prioritized_mids.size(); ++mid_idx)
    {
        int *const role = m_prioritized_mids[mid_idx];
        auto i = m_mark_map.find(role);
        int opp = i->second;
        int own = *i->first;

        if (m_own_robot[own].navigated())
        {
            Common::logWarning("robot {} is already navigated before marking opp {}", own, opp);
            continue;
        }

        if (opp == -1)
        {
            Common::Vec2 static_pos = m_sorted_zones[zone_idx]->best_pos;
            ++zone_idx;
            m_own_robot[own].face(m_world_state.ball.position);
            navigate(own, static_pos, VelocityProfile::aroom());
            m_own_robot[own].shoot(0);
        }
        else
        {
            mark(own, opp, Common::config().soccer.mark_distance);
        }
    }

    const Common::Vec2 attack_pos =
        m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 650);

    m_own_robot[m_attack].face(m_world_state.ball.position);
    navigate(m_attack, attack_pos, VelocityProfile::aroom());
    m_own_robot[m_attack].shoot(0);
}
} // namespace Tyr::Soccer
