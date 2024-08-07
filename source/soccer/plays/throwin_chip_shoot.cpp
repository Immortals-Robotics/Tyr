#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::throwinChipShoot()
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

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        if (mid == &m_mid5)
            continue;

        m_own_robot[*mid].face(m_world_state.ball.position);
        navigate(*mid, m_sorted_zones[zone_idx]->best_pos, VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);
        ++zone_idx;
    }

    if (m_timer.time().seconds() > 4)
    {
        circleBall(m_mid5,
                   Common::Vec2(oppGoal().x, Common::sign(m_world_state.ball.position.y) * 200.0f)
                       .angleWith(m_world_state.ball.position),
                   0, 15);
    }
    else
    {
        circleBall(m_mid5, oppGoal().angleWith(m_world_state.ball.position), 0, 0);
    }

    m_own_robot[m_attack].face(oppGoal());
    if (m_random_param < 0.5)
    {
        navigate(m_attack,
                 m_world_state.ball.position.pointOnConnectingLine(
                     Common::Vec2(oppGoal().x, Common::sign(-m_world_state.ball.position.x) * 2000.0f), 350),
                 VelocityProfile::mamooli(), NavigationFlagsForceBallMediumObstacle);
    }
    else
    {
        navigate(m_attack,
                 m_world_state.ball.position.pointOnConnectingLine(
                     Common::Vec2(oppGoal().x, Common::sign(m_world_state.ball.position.x) * 2000.0f), 350),
                 VelocityProfile::mamooli(), NavigationFlagsForceBallMediumObstacle);
    }
}
} // namespace Tyr::Soccer
