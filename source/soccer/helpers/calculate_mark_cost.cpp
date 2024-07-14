#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::calculateMarkCost(const int t_robot_num, const int t_opp)
{
    if (m_own_robot[t_robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        return -1;
    if (m_world_state.opp_robot[t_opp].seen_state == Common::SeenState::CompletelyOut)
        return -1;
    if (t_robot_num == m_attack)
        return -1;

    constexpr float kPredictT = 0.3f;

    const Common::Vec2 predicted_pos_own =
        m_own_robot[t_robot_num].state().position + m_own_robot[t_robot_num].state().velocity * kPredictT;
    const Common::Vec2 predicted_pos_opp =
        m_world_state.opp_robot[t_opp].position + m_world_state.opp_robot[t_opp].velocity * kPredictT;
    const float dis_pred = predicted_pos_own.distanceTo(predicted_pos_opp);

    const bool already_marked = std::any_of(m_mark_map.begin(), m_mark_map.end(), [t_robot_num, t_opp](const auto &pair)
                                            { return *pair.first == t_robot_num && pair.second == t_opp; });

    float cost_stay  = already_marked ? (dis_pred - 500.0f) / 1000.0f : 1.0f;
    float cost_reach = (dis_pred - 500.0f) / 10000.0f;

    cost_stay  = std::clamp(cost_stay, 0.0f, 1.0f);
    cost_reach = std::clamp(cost_reach, 0.0f, 1.0f);

    static constexpr float kWeightStay  = 0.0f;
    static constexpr float kWeightReach = 10.0f;
    static constexpr float kWeightSum   = kWeightStay + kWeightReach;

    const float cost = (kWeightStay * cost_stay + kWeightReach * cost_reach) / kWeightSum;

    return cost;
}
} // namespace Tyr::Soccer
