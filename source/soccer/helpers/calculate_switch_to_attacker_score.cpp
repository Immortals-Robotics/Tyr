#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::calculateSwitchToAttackerScore(const int t_robot_num)
{
    if (t_robot_num == m_gk)
        return -1;

    if (m_own_robot[t_robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        return -1;

    // Check if the robot is waiting for a pass and is in a suitable state to receive it
    const bool pass_receiver_role = t_robot_num == m_mid1 || t_robot_num == m_mid2 || t_robot_num == m_dmf;
    const bool one_touch_arriving = m_one_touch_detector[t_robot_num].IsArriving(45, 150);
    if (!m_is_defending && pass_receiver_role && one_touch_arriving)
        return 0;

    const float disToBall = m_own_robot[t_robot_num].state().position.distanceTo(m_world_state.ball.position);

    // check if robot is marking an opponent
    auto marked_opp = std::find_if(m_mark_map.begin(), m_mark_map.end(),
                                   [&](const auto &pair) { return *pair.first == t_robot_num; });
    if (m_is_defending && marked_opp != m_mark_map.end())
    {
        const float oppDisToBall =
            m_world_state.opp_robot[marked_opp->second].position.distanceTo(m_world_state.ball.position);

        if (oppDisToBall > 400 || disToBall > 400 || m_world_state.ball.velocity.length() > 500)
        {
            return 0;
        }
    }

    float dis_score = (5000.0f - disToBall) / 5000.0f;
    dis_score       = std::clamp(dis_score, 0.1f, 1.0f);

    // Calculate another score based on the robot's physical capabilities
    const auto &physical_status = Common::config().soccer.robot_physical_status[t_robot_num];

    float caps_score = 1.0f;
    if (!physical_status.is_3D_printed)
        caps_score *= 0.8f;
    if (!physical_status.has_direct_kick)
        caps_score *= 0.1f;
    if (!physical_status.has_chip_kick)
        caps_score *= 0.5f;

    // Return the combined score
    return dis_score * caps_score;
}
} // namespace Tyr::Soccer
