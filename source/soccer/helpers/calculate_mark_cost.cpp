#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::calculateMarkCost(int robot_num, int opp)
{
    if (OwnRobot[robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        return -1;
    if (m_world_state.opp_robot[opp].seen_state == Common::SeenState::CompletelyOut)
        return -1;

    const float predict_t = 0.3f;
    auto        predicted_pos_own =
        Common::Vec2(OwnRobot[robot_num].state().position.x + OwnRobot[robot_num].state().velocity.x * predict_t,
                     OwnRobot[robot_num].state().position.y + OwnRobot[robot_num].state().velocity.y * predict_t);
    auto predicted_pos_opp = Common::Vec2(m_world_state.opp_robot[opp].position.x + m_world_state.opp_robot[opp].velocity.x * predict_t,
                                          m_world_state.opp_robot[opp].position.y + m_world_state.opp_robot[opp].velocity.y * predict_t);

    auto dis_pred       = predicted_pos_own.distanceTo(predicted_pos_opp);
    bool already_marked = false;
    for (auto it = markMap.begin(); it != markMap.end(); ++it)
    {
        if (*it->first == robot_num)
        {
            already_marked = it->second == opp;
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

    float cost_attack = robot_num == attack ? 1.0f : 0.0f;

    score_stay  = std::min(1.0f, std::max(0.0f, score_stay));
    cost_reach  = std::min(1.0f, std::max(0.0f, cost_reach));
    cost_attack = std::min(1.0f, std::max(0.0f, cost_attack));

    auto cost = 1.0f - score_stay + cost_reach + cost_attack;

    return cost;
}
} // namespace Tyr::Soccer
