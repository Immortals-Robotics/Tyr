#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Mark2Ball(int robot_num, int opp, float dist)
{
    const float opp_predict_t = 0.15;
    dist                      = std::min(1500.0f, dist);

    Common::Vec2 predictedOpp = Common::Vec2(m_state.opp_robot[opp].position.x +
                                                 m_state.opp_robot[opp].velocity.x * opp_predict_t,
                                             m_state.opp_robot[opp].position.y +
                                                 m_state.opp_robot[opp].velocity.y * opp_predict_t);
    Common::Vec2 target       = predictedOpp.pointOnConnectingLine(m_state.ball.position, dist);

    OwnRobot[robot_num].face(m_state.ball.position);
    ERRTSetObstacles(robot_num, true, 1);
    ERRTNavigate2Point(robot_num, target, 100,
                       Common::refereeState().stop() ? VelocityProfile::Type::Aroom : VelocityProfile::Type::Mamooli);
}
} // namespace Tyr::Soccer
