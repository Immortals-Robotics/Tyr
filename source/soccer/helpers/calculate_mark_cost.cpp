#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::calculateMarkCost(int t_robot_num, int t_opp)
{
    if (m_own_robot[t_robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        return -1;
    if (m_world_state.opp_robot[t_opp].seen_state == Common::SeenState::CompletelyOut)
        return -1;

    const float predict_t  = 0.3f;
    auto predicted_pos_own =
        m_own_robot[t_robot_num].state().position + m_own_robot[t_robot_num].state().velocity * predict_t;
    auto predicted_pos_opp = m_world_state.opp_robot[t_opp].position + m_world_state.opp_robot[t_opp].velocity * predict_t;

    auto dis_pred       = predicted_pos_own.distanceTo(predicted_pos_opp);
    bool already_marked = false;
    for (auto it = m_mark_map.begin(); it != m_mark_map.end(); ++it)
    {
        if (*it->first == t_robot_num)
        {
            already_marked = it->second == t_opp;
            break;
        }
    }

    float score_stay;
    if (!already_marked)
        score_stay = 0.0f;
    else
    {
        if (dis_pred < 500)
            score_stay = 1.0f;
        else
            score_stay = 1.0f - (dis_pred - 500.0f) / 1000.0f;
    }

    float cost_reach;
    if (dis_pred < 500)
        cost_reach = 0.0f;
    else
        cost_reach = (dis_pred - 500.0f) / 1000.0f;

    float cost_attack = t_robot_num == m_attack ? 1.0f : 0.0f;

    score_stay  = std::min(1.0f, std::max(0.0f, score_stay));
    cost_reach  = std::min(1.0f, std::max(0.0f, cost_reach));
    cost_attack = std::min(1.0f, std::max(0.0f, cost_attack));

    auto cost = 1.0f - score_stay + cost_reach + cost_attack;

    return cost;
}
} // namespace Tyr::Soccer
