#include "../ai.h"

#include "../tactics/mark.h"

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

    gkHi(m_own_robot[m_gk]);
    defHi(m_own_robot[m_def1], m_own_robot[m_def2], nullptr);

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
            m_own_robot[own].navigate(static_pos, VelocityProfile::aroom());
            m_own_robot[own].shoot(0);
        }
        else
        {
            MarkTactic{m_world_state.opp_robot[opp], Common::config().soccer.mark_distance}.execute(m_own_robot[own]);
        }
    }

    const Common::Vec2 attack_pos =
        m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(Field::ownGoal()), 650);

    m_own_robot[m_attack].face(m_world_state.ball.position);
    m_own_robot[m_attack].navigate(attack_pos, VelocityProfile::aroom());
    m_own_robot[m_attack].shoot(0);
}
} // namespace Tyr::Soccer
