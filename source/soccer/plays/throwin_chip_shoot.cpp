#include "../ai.h"

#include "../tactics/circle_ball.h"
#include "../tactics/def.h"
#include "../tactics/gk.h"

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
        m_own_robot[*mid].navigate(m_sorted_zones[zone_idx]->best_pos, VelocityProfile::mamooli(),
                                   NavigationFlags::BallObstacle);
        ++zone_idx;
    }

    if (m_timer.time().seconds() > 4)
    {
        CircleBallTactic{Common::Vec2(Field::oppGoal().x, Common::sign(m_world_state.ball.position.y) * 200.0f)
                             .angleWith(m_world_state.ball.position),
                         0, 15}
            .execute(m_own_robot[m_mid5]);
    }
    else
    {
        CircleBallTactic{Field::oppGoal().angleWith(m_world_state.ball.position), 0, 0}.execute(m_own_robot[m_mid5]);
    }

    m_own_robot[m_attack].face(Field::oppGoal());
    if (m_random_param < 0.5)
    {
        m_own_robot[m_attack].navigate(
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(Field::oppGoal().x, Common::sign(-m_world_state.ball.position.x) * 2000.0f), 350),
            VelocityProfile::mamooli(), NavigationFlags::BallMediumObstacle);
    }
    else
    {
        m_own_robot[m_attack].navigate(
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(Field::oppGoal().x, Common::sign(m_world_state.ball.position.x) * 2000.0f), 350),
            VelocityProfile::mamooli(), NavigationFlags::BallMediumObstacle);
    }
}
} // namespace Tyr::Soccer
