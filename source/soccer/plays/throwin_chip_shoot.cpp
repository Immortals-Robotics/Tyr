#include "../ai.h"

#include "../tactics/gk.h"
#include "../tactics/def.h"

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

    GkTactic{}.execute(m_own_robot[m_gk]);

    DefTactic{1}.execute(m_own_robot[m_def1]);
    DefTactic{2}.execute(m_own_robot[m_def2]);

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        if (mid == &m_mid5)
            continue;

        m_own_robot[*mid].face(m_world_state.ball.position);
        m_own_robot[*mid].navigate(m_sorted_zones[zone_idx]->best_pos, VelocityProfile::mamooli(), NavigationFlags::ForceBallObstacle);
        ++zone_idx;
    }

    if (m_timer.time().seconds() > 4)
    {
        circleBall(m_own_robot[m_mid5],
                   Common::Vec2(Field::oppGoal().x, Common::sign(m_world_state.ball.position.y) * 200.0f)
                       .angleWith(m_world_state.ball.position),
                   0, 15);
    }
    else
    {
        circleBall(m_own_robot[m_mid5], Field::oppGoal().angleWith(m_world_state.ball.position), 0, 0);
    }

    m_own_robot[m_attack].face(Field::oppGoal());
    if (m_random_param < 0.5)
    {
        m_own_robot[m_attack].navigate(
                 m_world_state.ball.position.pointOnConnectingLine(
                     Common::Vec2(Field::oppGoal().x, Common::sign(-m_world_state.ball.position.x) * 2000.0f), 350),
                 VelocityProfile::mamooli(), NavigationFlags::ForceBallMediumObstacle);
    }
    else
    {
        m_own_robot[m_attack].navigate(
                 m_world_state.ball.position.pointOnConnectingLine(
                     Common::Vec2(Field::oppGoal().x, Common::sign(m_world_state.ball.position.x) * 2000.0f), 350),
                 VelocityProfile::mamooli(), NavigationFlags::ForceBallMediumObstacle);
    }
}
} // namespace Tyr::Soccer
