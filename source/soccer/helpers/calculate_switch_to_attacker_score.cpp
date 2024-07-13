#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::calculateSwitchToAttackerScore(const int t_robot_num)
{
    if (t_robot_num == m_gk || t_robot_num == m_def)
        return -1;

    if (m_own_robot[t_robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        return -1;

    if (!m_is_defending && m_one_touch_detector[t_robot_num].IsArriving(45, 150))
        return 0;

    const float disToBall = m_own_robot[t_robot_num].state().position.distanceTo(m_world_state.ball.position);

    auto marked_opp = std::find_if(m_mark_map.begin(), m_mark_map.end(),
                                   [&](const auto &pair) { return *pair.first == t_robot_num; });
    if (m_is_defending && marked_opp != m_mark_map.end())
    {
        const float oppDisToBall =
            m_world_state.opp_robot[marked_opp->second].position.distanceTo(m_world_state.ball.position);

        if (oppDisToBall < 400 && disToBall < 400 && m_world_state.ball.velocity.length() < 500)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }

    float dis_score = 0.1f + (5000.0f - disToBall) / 5000.0f;
    dis_score       = std::clamp(dis_score, 0.0f, 1.0f);

    // check capabilities
    const auto &physical_status = Common::config().soccer.robot_physical_status[t_robot_num];

    float cap_score = 1.0f;
    if (!physical_status.is_3D_printed)
        cap_score *= 0.8f;
    if (!physical_status.has_direct_kick)
        cap_score *= 0.1f;
    if (!physical_status.has_chip_kick)
        cap_score *= 0.5f;

    return dis_score * cap_score;
}
} // namespace Tyr::Soccer
