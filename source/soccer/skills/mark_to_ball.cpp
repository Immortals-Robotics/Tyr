#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::mark2Ball(const int t_robot_num, const int t_opp, float t_dist)
{
    const float opp_predict_t = 0.15f;
    t_dist                    = std::min(1500.0f, t_dist);

    Common::Vec2 predictedOpp =
        m_world_state.opp_robot[t_opp].position + m_world_state.opp_robot[t_opp].velocity * opp_predict_t;
    Common::Vec2 target = predictedOpp.pointOnConnectingLine(m_world_state.ball.position, t_dist);

    m_own_robot[t_robot_num].face(m_world_state.ball.position);
    m_own_robot[t_robot_num].navigate(target, VelocityProfile::mamooli(), NavigationFlags::ForceBallObstacle);
}
} // namespace Tyr::Soccer
