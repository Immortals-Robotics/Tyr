#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::calculateSwitchToAttackerScore(int robot_num)
{
    if (m_own_robot[robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        return -1;

    if (robot_num != mid1 && robot_num != mid2)
        return -1;

    if (!m_is_defending && m_one_touch_detector[robot_num].IsArriving(45, 150))
        return -1;

    float currAttBallDis = m_own_robot[attack].state().position.distanceTo(m_world_state.ball.position);

    if (m_own_robot[attack].state().seen_state == Common::SeenState::CompletelyOut)
        currAttBallDis = 20000;

    int marked_id = -1;
    for (auto it = m_mark_map.begin(); it != m_mark_map.end(); ++it)
    {
        if (*it->first == robot_num)
        {
            marked_id = it->second;
            break;
        }
    }

    if (m_is_defending && marked_id != -1)
    {
        int opp = marked_id;
        if ((m_world_state.opp_robot[opp].position.distanceTo(m_world_state.ball.position) < 400) &&
            (m_own_robot[robot_num].state().position.distanceTo(m_world_state.ball.position) < 400) &&
            (currAttBallDis > 600) && (m_world_state.ball.velocity.length() < 500))
        {
            return 0;
        }
        else
            return -1;
    }

    float disToBall = m_own_robot[robot_num].state().position.distanceTo(m_world_state.ball.position);
    if (disToBall > currAttBallDis - 500)
        return 0;

    float dis_score = (currAttBallDis - disToBall - 500) / 1000.0f;
    dis_score       = std::min(1.0f, std::max(0.0f, dis_score));

    return dis_score;
}
} // namespace Tyr::Soccer
