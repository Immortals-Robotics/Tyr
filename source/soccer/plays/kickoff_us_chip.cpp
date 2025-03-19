#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoffUsChip()
{
    m_is_defending = false;

    m_assignments.clear();
    createGkAssignment();
    createDefAssignments();
    createMidAssignments();
    createAttackAssignment();
    assignRoles();

    gkHi(m_gk);
    defHi(m_def1, m_def2, nullptr);

    m_own_robot[m_mid5].face(m_world_state.ball.position);
    m_own_robot[m_mid5].navigate(
             m_world_state.ball.position.pointOnConnectingLine(
                 ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
             VelocityProfile::mamooli(), NavigationFlags::ForceBallObstacle);

    if (m_timer.time().seconds() < 0.5)
    {
        if (m_own_robot[m_mid1].state().position.y > m_own_robot[m_mid2].state().position.y)
        {
            std::swap(m_mid1, m_mid2);
        }
    }

    m_own_robot[m_mid2].face(oppGoal());
    m_own_robot[m_mid2].navigate(Common::Vec2(m_world_state.ball.position.x + m_side * 150, (Common::field().height - 300)),
             VelocityProfile::mamooli(), NavigationFlags::ForceBallObstacle);

    m_own_robot[m_mid1].face(oppGoal());
    m_own_robot[m_mid1].navigate(Common::Vec2(m_world_state.ball.position.x + m_side * 150, -(Common::field().height - 300)),
             VelocityProfile::mamooli(), NavigationFlags::ForceBallObstacle);

    int zone_idx = 0;
    for (const auto& mid : m_prioritized_mids)
    {
        if (m_own_robot[*mid].navigated())
            continue;

        m_own_robot[*mid].face(m_world_state.ball.position);
        m_own_robot[*mid].navigate(m_sorted_zones[zone_idx]->best_pos, VelocityProfile::mamooli());
        ++zone_idx;
    }

    Common::Vec2 chip_target = Common::Vec2(-m_side * 2000, 0);
    Common::logDebug("can kick ball: {}", m_ref_state.canKickBall());
    if (m_ref_state.canKickBall())
    {
        // TODO: why isn't this circleBall?
        attacker(m_attack, chip_target.angleWith(m_world_state.ball.position), 0, 80, 0, 1);
    }
    else
    {
        circleBall(m_attack, chip_target.angleWith(m_world_state.ball.position), 0, 0);
    }
}
} // namespace Tyr::Soccer
