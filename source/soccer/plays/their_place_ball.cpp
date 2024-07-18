#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::theirPlaceBall()
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

    {
        m_own_robot[m_mid5].face(m_world_state.ball.position);
        navigate(m_mid5,
                 m_world_state.ball.position.pointOnConnectingLine(
                     ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
                 VelocityProfile::aroom());

        m_own_robot[m_mid2].face(m_world_state.ball.position);
        navigate(m_mid2,
                 m_world_state.ball.position.circleAroundPoint(
                     Common::Angle::fromDeg(-20) + m_world_state.ball.position.angleWith(ownGoal()), 1090),
                 VelocityProfile::aroom());

        m_own_robot[m_mid1].face(m_world_state.ball.position);
        navigate(m_mid1,
                 m_world_state.ball.position.circleAroundPoint(
                     Common::Angle::fromDeg(20) + m_world_state.ball.position.angleWith(ownGoal()), 1090),
                 VelocityProfile::aroom());

        m_own_robot[m_attack].face(m_world_state.ball.position);
        navigate(m_attack,
                 m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 1090),
                 VelocityProfile::aroom());
    }
}
} // namespace Tyr::Soccer
