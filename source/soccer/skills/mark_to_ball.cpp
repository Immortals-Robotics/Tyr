#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Mark2Ball(int robot_num, int opp, float dist)
{
    const float opp_predict_t = 0.15;
    dist                      = std::min(1500.0f, dist);

    Common::Vec2 predictedOpp =
        Common::Vec2(m_world_state.opp_robot[opp].position.x + m_world_state.opp_robot[opp].velocity.x * opp_predict_t,
                     m_world_state.opp_robot[opp].position.y + m_world_state.opp_robot[opp].velocity.y * opp_predict_t);
    Common::Vec2 target = predictedOpp.pointOnConnectingLine(m_world_state.ball.position, dist);

    OwnRobot[robot_num].face(m_world_state.ball.position);
    setObstacles(robot_num, true);
    navigate(robot_num, target, VelocityProfile::Type::Mamooli);
}
} // namespace Tyr::Soccer
