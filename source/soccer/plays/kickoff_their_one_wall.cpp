#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoffTheirOneWall()
{
    m_is_defending = true;

    m_assignments.clear();
    createGkAssignment();
    createDefAssignments();
    createMidAssignments();
    createAttackAssignment();
    assignRoles();

    gkHi(m_gk);
    defHi(m_def1, m_def2, nullptr);

    int zone_idx = 0;
    m_own_robot[m_mid5].face(m_world_state.ball.position);
    navigate(m_mid5, m_sorted_zones[zone_idx++]->best_pos, VelocityProfile::mamooli());

    int indexP = -1;
    int indexN = -1;

    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        if ((std::fabs(m_world_state.opp_robot[i].position.x) < 1500) &&
            (std::fabs(m_world_state.opp_robot[i].position.y) > 600) &&
            (m_world_state.opp_robot[i].seen_state != Common::SeenState::CompletelyOut))
        {
            if (m_world_state.opp_robot[i].position.y > 0)
                indexP = i;
            if (m_world_state.opp_robot[i].position.y < 0)
                indexN = i;
        }
    }

    Common::logDebug("{}    {}", indexN, indexP);

    if (indexN != -1)
    {
        m_own_robot[m_mid1].face(oppGoal());
        navigate(m_mid1, m_world_state.opp_robot[indexN].position.pointOnConnectingLine(
                             ownGoal(), (std::fabs(m_world_state.opp_robot[indexN].position.x) + 14) * 1.5));
    }
    else
    {
        m_own_robot[m_mid1].face(m_world_state.ball.position);
        navigate(m_mid1, m_sorted_zones[zone_idx++]->best_pos);
    }

    if (indexP != -1)
    {
        m_own_robot[m_mid2].face(oppGoal());
        navigate(m_mid2, m_world_state.opp_robot[indexP].position.pointOnConnectingLine(
                             ownGoal(), (std::fabs(m_world_state.opp_robot[indexP].position.x) + 14) * 1.5));
    }
    else
    {
        m_own_robot[m_mid2].face(m_world_state.ball.position);
        navigate(m_mid2, m_sorted_zones[zone_idx++]->best_pos);
    }

    defenceWall(m_attack, true);
}
} // namespace Tyr::Soccer
