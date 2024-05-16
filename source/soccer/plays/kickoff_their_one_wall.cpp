#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoff_their_one_wall()
{
    GKHi(gk, 1);
    DefHi(def, rw, lw, nullptr, true);

    OwnRobot[dmf].face(m_world_state.ball.position);
    navigate(dmf,
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
        if (side == -1)
        {
            OwnRobot[mid1].face(oppGoal());
            navigate(mid1, m_world_state.opp_robot[indexN].position.pointOnConnectingLine(
                               ownGoal(), (std::fabs(m_world_state.opp_robot[indexN].position.x) + 14) * 1.5));
            markMap[&mid1] = indexN;
        }
        else
        {
            OwnRobot[mid2].face(oppGoal());
            navigate(mid2, m_world_state.opp_robot[indexN].position.pointOnConnectingLine(
                               ownGoal(), (std::fabs(m_world_state.opp_robot[indexN].position.x) + 14) * 1.5));
            markMap[&mid2] = indexN;
        }
    }
    else
    {
        if (side == -1)
        {
            OwnRobot[mid1].face(m_world_state.ball.position);
            navigate(mid1,
                     m_world_state.ball.position.circleAroundPoint(
                         Common::Angle::fromDeg(20.0f) + m_world_state.ball.position.angleWith(ownGoal()), 790.0f));
            markMap[&mid1] = -1;
        }
        else
        {
            OwnRobot[mid2].face(m_world_state.ball.position);
            navigate(mid2,
                     m_world_state.ball.position.circleAroundPoint(
                         Common::Angle::fromDeg(-20.0f) + m_world_state.ball.position.angleWith(ownGoal()), 790.0f));
            markMap[&mid2] = -1;
        }
    }

    if (indexP != -1)
    {
        if (side == 1)
        {
            OwnRobot[mid1].face(oppGoal());
            navigate(mid1, m_world_state.opp_robot[indexP].position.pointOnConnectingLine(
                               ownGoal(), (std::fabs(m_world_state.opp_robot[indexP].position.x) + 14) * 1.5));
            markMap[&mid1] = indexP;
        }
        else
        {
            OwnRobot[mid2].face(oppGoal());
            navigate(mid2, m_world_state.opp_robot[indexP].position.pointOnConnectingLine(
                               ownGoal(), (std::fabs(m_world_state.opp_robot[indexP].position.x) + 14) * 1.5));
            markMap[&mid2] = indexP;
        }
    }
    else
    {
        if (side == 1)
        {
            OwnRobot[mid1].face(m_world_state.ball.position);
            navigate(mid1, m_world_state.ball.position.circleAroundPoint(
                               Common::Angle::fromDeg(20) + m_world_state.ball.position.angleWith(ownGoal()), 790));
            markMap[&mid1] = -1;
        }
        else
        {
            OwnRobot[mid2].face(m_world_state.ball.position);
            navigate(mid2, m_world_state.ball.position.circleAroundPoint(
                               Common::Angle::fromDeg(-20) + m_world_state.ball.position.angleWith(ownGoal()), 790));
            markMap[&mid2] = -1;
        }
    }

    DefenceWall(attack, true);
}
} // namespace Tyr::Soccer
