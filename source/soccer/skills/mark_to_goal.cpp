#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::mark2Goal(int t_robot_num, int t_opp, float t_dist)
{
    const float opp_predict_t = 0.15;
    t_dist                    = std::min(1500.0f, t_dist);

    Common::Vec2 predictedOpp =
        m_world_state.opp_robot[t_opp].position + m_world_state.opp_robot[t_opp].velocity * opp_predict_t;
    Common::Vec2 target = predictedOpp.pointOnConnectingLine(ownGoal(), t_dist);

    m_own_robot[t_robot_num].face(m_world_state.ball.position);
    navigate(t_robot_num, target, VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);
}
} // namespace Tyr::Soccer
