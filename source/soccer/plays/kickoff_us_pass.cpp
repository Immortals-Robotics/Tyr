#include "../ai.h"

#include "../tactics/circle_ball.h"
#include "../tactics/def.h"
#include "../tactics/gk.h"

namespace Tyr::Soccer
{
void Ai::kickoffUsPass()
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

    NavigationFlags non_attack_flags = NavigationFlags::BallObstacle | NavigationFlags::TheirHalf;

    m_own_robot[m_mid2].face(m_world_state.ball.position);
    m_own_robot[m_mid2].navigate(m_world_state.ball.position.pointOnConnectingLine(
                                     Field::ownGoal(), 1000.0f),
                                 VelocityProfile::mamooli(), non_attack_flags);

    m_own_robot[m_mid5].face(Field::oppGoal());
    m_own_robot[m_mid5].navigate(
        Common::Vec2(m_world_state.ball.position.x + m_side * 150, (Common::field().height - 300)),
        VelocityProfile::mamooli(), non_attack_flags);

    m_own_robot[m_mid1].face(Field::oppGoal());
    m_own_robot[m_mid1].navigate(
        Common::Vec2(m_world_state.ball.position.x + m_side * 150, -(Common::field().height - 300)),
        VelocityProfile::mamooli(), non_attack_flags);

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        if (m_own_robot[*mid].navigated())
            continue;

        m_own_robot[*mid].face(m_world_state.ball.position);
        m_own_robot[*mid].navigate(m_sorted_zones[zone_idx]->best_pos, VelocityProfile::mamooli(), non_attack_flags);
        ++zone_idx;
    }

    Common::Vec2 chip_target = m_own_robot[m_mid5].state().position;
    Common::logDebug("can kick ball: {}", m_ref_state.canKickBall());

    if (m_timer.time().seconds() < 2.0f)
    {
        CircleBallTactic{Field::oppGoal().angleWith(m_world_state.ball.position), 0, 0}.execute(m_own_robot[m_attack]);
    }
    else
    {
        const float shoot_pow = m_ref_state.canKickBall() ? 5000.0f : 0.0f;
        CircleBallTactic{chip_target.angleWith(m_world_state.ball.position), shoot_pow, 0}.execute(m_own_robot[m_attack]);
    }
}
} // namespace Tyr::Soccer
