#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoffTheirOneWall()
{
    gkHi(m_gk);
    defHi(m_def, m_rw, m_lw, nullptr);

    m_own_robot[m_dmf].face(m_world_state.ball.position);
    navigate(m_dmf,
             m_world_state.ball.position.pointOnConnectingLine(
                 ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 2.0f),
             VelocityProfile::mamooli());

    int indexP = -1;
    int indexN = -1;

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
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
        if (m_side == -1)
        {
            m_own_robot[m_mid1].face(oppGoal());
            navigate(m_mid1, m_world_state.opp_robot[indexN].position.pointOnConnectingLine(
                               ownGoal(), (std::fabs(m_world_state.opp_robot[indexN].position.x) + 14) * 1.5));
            m_mark_map[&m_mid1] = indexN;
        }
        else
        {
            m_own_robot[m_mid2].face(oppGoal());
            navigate(m_mid2, m_world_state.opp_robot[indexN].position.pointOnConnectingLine(
                               ownGoal(), (std::fabs(m_world_state.opp_robot[indexN].position.x) + 14) * 1.5));
            m_mark_map[&m_mid2] = indexN;
        }
    }
    else
    {
        if (m_side == -1)
        {
            m_own_robot[m_mid1].face(m_world_state.ball.position);
            navigate(m_mid1,
                     m_world_state.ball.position.circleAroundPoint(
                         Common::Angle::fromDeg(20.0f) + m_world_state.ball.position.angleWith(ownGoal()), 790.0f));
            m_mark_map[&m_mid1] = -1;
        }
        else
        {
            m_own_robot[m_mid2].face(m_world_state.ball.position);
            navigate(m_mid2,
                     m_world_state.ball.position.circleAroundPoint(
                         Common::Angle::fromDeg(-20.0f) + m_world_state.ball.position.angleWith(ownGoal()), 790.0f));
            m_mark_map[&m_mid2] = -1;
        }
    }

    if (indexP != -1)
    {
        if (m_side == 1)
        {
            m_own_robot[m_mid1].face(oppGoal());
            navigate(m_mid1, m_world_state.opp_robot[indexP].position.pointOnConnectingLine(
                               ownGoal(), (std::fabs(m_world_state.opp_robot[indexP].position.x) + 14) * 1.5));
            m_mark_map[&m_mid1] = indexP;
        }
        else
        {
            m_own_robot[m_mid2].face(oppGoal());
            navigate(m_mid2, m_world_state.opp_robot[indexP].position.pointOnConnectingLine(
                               ownGoal(), (std::fabs(m_world_state.opp_robot[indexP].position.x) + 14) * 1.5));
            m_mark_map[&m_mid2] = indexP;
        }
    }
    else
    {
        if (m_side == 1)
        {
            m_own_robot[m_mid1].face(m_world_state.ball.position);
            navigate(m_mid1, m_world_state.ball.position.circleAroundPoint(
                               Common::Angle::fromDeg(20) + m_world_state.ball.position.angleWith(ownGoal()), 790));
            m_mark_map[&m_mid1] = -1;
        }
        else
        {
            m_own_robot[m_mid2].face(m_world_state.ball.position);
            navigate(m_mid2, m_world_state.ball.position.circleAroundPoint(
                               Common::Angle::fromDeg(-20) + m_world_state.ball.position.angleWith(ownGoal()), 790));
            m_mark_map[&m_mid2] = -1;
        }
    }

    defenceWall(m_attack, true);
}
} // namespace Tyr::Soccer
