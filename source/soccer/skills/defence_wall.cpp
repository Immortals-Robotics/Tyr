#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::defenceWall(const int t_robot_num, const bool t_kick_off)
{
    float x     = -m_side * m_world_state.ball.position.x;
    float tetta = -0.000003f * x * x + 0.0016f * x + 90.0f;
    if (t_kick_off)
    {
        tetta = 14.0f;
    }
    Common::logDebug("wall limit: {}", tetta);

    Common::Vec2 target;

    int index = findKickerOpp(-1);
    if (index == -1)
    {
        target = m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 730);
    }
    else
    {
        target = m_world_state.opp_robot[index].position.pointOnConnectingLine(
            m_world_state.ball.position,
            590 + m_world_state.ball.position.distanceTo(m_world_state.opp_robot[index].position));
    }

    Common::logDebug("{}", index);

    Common::Angle ballAngle = m_world_state.ball.position.angleWith(target);
    Common::Angle firstLeg  = m_world_state.ball.position.angleWith(
        Common::Vec2(ownGoal().x, Common::sign(m_world_state.ball.position.y) * (350.0f)));
    Common::Angle secLeg =
        firstLeg - Common::Angle::fromDeg(tetta * Common::sign(m_world_state.ball.position.y) * m_side);

    Common::logDebug("ball: {}    f: {}    s: {}", ballAngle, firstLeg, secLeg);

    bool isOut =
        false; // std::fabs((std::fabs(NormalizeAngle(ballAngle-firstLeg))+std::fabs(NormalizeAngle(ballAngle-secLeg)))
               // - tetta ) > 1.0f;

    if (isOut)
    {
        target = m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 730);
    }

    m_own_robot[t_robot_num].face(m_world_state.ball.position);
    m_own_robot[t_robot_num].navigate(target);
}
} // namespace Tyr::Soccer
