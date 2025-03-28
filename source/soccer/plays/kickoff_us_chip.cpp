#include "../ai.h"

#include "../tactics/circle_ball.h"
#include "../tactics/def.h"
#include "../tactics/gk.h"

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

    GkTactic{}.execute(m_own_robot[m_gk]);

    DefTactic{1}.execute(m_own_robot[m_def1]);
    DefTactic{2}.execute(m_own_robot[m_def2]);

    m_own_robot[m_mid5].face(m_world_state.ball.position);
    m_own_robot[m_mid5].navigate(m_world_state.ball.position.pointOnConnectingLine(
                                     Field::ownGoal(), 1000.0f),
                                 VelocityProfile::mamooli(), NavigationFlags::BallObstacle);

    if (m_timer.time().seconds() < 0.5)
    {
        if (m_own_robot[m_mid1].state().position.y > m_own_robot[m_mid2].state().position.y)
        {
            std::swap(m_mid1, m_mid2);
        }
    }

    m_own_robot[m_mid2].face(Field::oppGoal());
    m_own_robot[m_mid2].navigate(
        Common::Vec2(m_world_state.ball.position.x + m_side * 150, (Common::field().height - 300)),
        VelocityProfile::mamooli(), NavigationFlags::BallObstacle);

    m_own_robot[m_mid1].face(Field::oppGoal());
    m_own_robot[m_mid1].navigate(
        Common::Vec2(m_world_state.ball.position.x + m_side * 150, -(Common::field().height - 300)),
        VelocityProfile::mamooli(), NavigationFlags::BallObstacle);

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        if (m_own_robot[*mid].navigated())
            continue;

        m_own_robot[*mid].face(m_world_state.ball.position);
        m_own_robot[*mid].navigate(m_sorted_zones[zone_idx]->best_pos, VelocityProfile::mamooli());
        ++zone_idx;
    }

    Common::Vec2 chip_target = Common::Vec2(-m_side * 2000, 0);
    Common::logDebug("can kick ball: {}", m_ref_state.canKickBall());


    const float chip_pow = m_ref_state.canKickBall() ? 80.0f : 0.0f;
    CircleBallTactic{chip_target.angleWith(m_world_state.ball.position), 0, chip_pow}.execute(m_own_robot[m_attack]);
}
} // namespace Tyr::Soccer
