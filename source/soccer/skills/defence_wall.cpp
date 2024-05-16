#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::DefenceWall(int robot_num, bool kickOff)
{
    float x     = -side * m_world_state.ball.position.x;
    float tetta = -0.000003f * x * x + 0.0016f * x + 90.0f;
    if (kickOff)
    {
        tetta = 14.0f;
    }
    Common::logDebug("wall limit: {}", tetta);

    Common::Vec2 target;

    int index = findKickerOpp(-1);
    if (index == -1)
    {
        target = m_world_state.ball.position.circleAroundPoint(
            m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)),
            730);
    }
    else
    {
        target = m_world_state.opp_robot[index].position.pointOnConnectingLine(
            m_world_state.ball.position,
            590 + m_world_state.ball.position.distanceTo(m_world_state.opp_robot[index].position));
    }

    Common::logDebug("{}", index);

    Common::Angle ballAngle = m_world_state.ball.position.angleWith(target);
    Common::Angle firstLeg  = m_world_state.ball.position.angleWith(Common::Vec2(
        side * Common::field().width, Common::sign(m_world_state.ball.position.y) * (350.0f)));
    Common::Angle secLeg =
        firstLeg - Common::Angle::fromDeg(tetta * Common::sign(m_world_state.ball.position.y) * side);

    Common::logDebug("ball: {}    f: {}    s: {}", ballAngle, firstLeg, secLeg);

    bool isOut =
        false; // std::fabs((std::fabs(NormalizeAngle(ballAngle-firstLeg))+std::fabs(NormalizeAngle(ballAngle-secLeg)))
               // - tetta ) > 1.0f;

    if (isOut)
    {
        target = m_world_state.ball.position.circleAroundPoint(
            m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)),
            730);
    }

    OwnRobot[robot_num].face(m_world_state.ball.position);
    setObstacles(robot_num);
    navigate(robot_num, target);
}
} // namespace Tyr::Soccer
