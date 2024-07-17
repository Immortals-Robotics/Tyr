#include "../ai.h"

namespace Tyr::Soccer
{
int Ai::markRoleCost(const int t_robot_idx, const Assignment &t_assignment) const
{
    const Common::RobotState &own_robot = m_own_robot[t_robot_idx].state();
    const Common::RobotState &opp_robot = m_world_state.opp_robot[t_assignment.target_idx];

    if (own_robot.seen_state == Common::SeenState::CompletelyOut)
        return -1;
    if (opp_robot.seen_state == Common::SeenState::CompletelyOut)
        return -1;
    if (t_robot_idx == m_attack)
        return -1;

    constexpr float kPredictT = 0.3f;

    const Common::Vec2 predicted_pos_own = own_robot.position + own_robot.velocity * kPredictT;
    const Common::Vec2 predicted_pos_opp = opp_robot.position + opp_robot.velocity * kPredictT;
    const float        dis_pred          = predicted_pos_own.distanceTo(predicted_pos_opp);

    const bool already_marked = t_robot_idx == t_assignment.currentAssignee();

    float cost_stay  = already_marked ? (dis_pred - 500.0f) / 1000.0f : 1.0f;
    float cost_reach = (dis_pred - 500.0f) / 10000.0f;

    cost_stay  = std::clamp(cost_stay, 0.0f, 1.0f);
    cost_reach = std::clamp(cost_reach, 0.0f, 1.0f);

    static constexpr float kWeightStay  = 0.0f;
    static constexpr float kWeightReach = 1.0f;

    const float cost = 10000.0f * kWeightStay * cost_stay + kWeightReach * cost_reach;

    return cost;
}
} // namespace Tyr::Soccer
