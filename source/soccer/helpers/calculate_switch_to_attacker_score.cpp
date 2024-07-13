#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::calculateSwitchToAttackerScore(const int t_robot_num)
{
    if (t_robot_num == m_gk || t_robot_num == m_def || t_robot_num == m_attack)
        return -1;

    if (m_own_robot[t_robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        return -1;

    if (!m_is_defending && m_one_touch_detector[t_robot_num].IsArriving(45, 150))
        return -1;

    float currAttBallDis = m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position);
    if (m_own_robot[m_attack].state().seen_state == Common::SeenState::CompletelyOut)
        currAttBallDis = std::numeric_limits<float>::max();

    const float disToBall = m_own_robot[t_robot_num].state().position.distanceTo(m_world_state.ball.position);

    int marked_id = -1;
    for (auto it = m_mark_map.begin(); it != m_mark_map.end(); ++it)
    {
        if (*it->first == t_robot_num)
        {
            marked_id = it->second;
            break;
        }
    }

    if (m_is_defending && marked_id != -1)
    {
        const float oppDisToBall = m_world_state.opp_robot[marked_id].position.distanceTo(m_world_state.ball.position);

        if (oppDisToBall < 400 && disToBall < 400 && currAttBallDis > 600 && m_world_state.ball.velocity.length() < 500)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }

    if (disToBall > currAttBallDis - 500)
        return 0;

    float dis_score = (currAttBallDis - disToBall - 500) / 1000.0f;
    dis_score       = std::clamp(dis_score, 0.0f, 1.0f);

    return dis_score;
}
} // namespace Tyr::Soccer
