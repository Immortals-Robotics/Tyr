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
        m_own_robot[m_mid5].face(oppGoal());
        navigate(m_mid5, m_world_state.opp_robot[indexN].position.pointOnConnectingLine(
                             ownGoal(), (std::fabs(m_world_state.opp_robot[indexN].position.x) + 14) * 1.5));
    }
    if (indexP != -1)
    {
        m_own_robot[m_mid1].face(oppGoal());
        navigate(m_mid1, m_world_state.opp_robot[indexP].position.pointOnConnectingLine(
                             ownGoal(), (std::fabs(m_world_state.opp_robot[indexP].position.x) + 14) * 1.5));
    }

    int zone_idx = 0;
    for (const auto& mid : m_prioritized_mids)
    {
        if (m_own_robot[*mid].navigated())
            continue;

        m_own_robot[*mid].face(m_world_state.ball.position);
        navigate(*mid, m_sorted_zones[zone_idx]->best_pos, VelocityProfile::mamooli());
        ++zone_idx;
    }

    defenceWall(m_attack, true);
}
} // namespace Tyr::Soccer
