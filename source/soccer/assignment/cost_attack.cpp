#include "../ai.h"

namespace Tyr::Soccer
{
int Ai::attackRoleCost(const int t_robot_idx, const Assignment &t_assignment)
{
    // Check if the robot is waiting for a pass and is in a suitable state to receive it
    const bool pass_receiver_role = t_robot_idx == m_mid1 || t_robot_idx == m_mid2 || t_robot_idx == m_mid5;
    const bool one_touch_arriving = m_one_touch_detector[t_robot_idx].isArriving(45, 150);
    if (!m_is_defending && pass_receiver_role && one_touch_arriving)
        return -1;

    const float dis_to_ball = m_own_robot[t_robot_idx].state().position.distanceTo(m_world_state.ball.position);

    // check if robot is marking an opponent
    float mark_score = 1.0f;
    auto  marked_opp = std::find_if(m_mark_map.begin(), m_mark_map.end(),
                                    [&](const auto &pair) { return *pair.first == t_robot_idx; });
    if (m_is_defending && marked_opp != m_mark_map.end())
    {
        const float oppDisToBall =
            m_world_state.opp_robot[marked_opp->second].position.distanceTo(m_world_state.ball.position);

        if (oppDisToBall > 400 || dis_to_ball > 400 || m_world_state.ball.velocity.length() > 500)
        {
            mark_score = 0.1f;
        }
        else
        {
            mark_score = 0.8f;
        }
    }

    float dis_score = (5000.0f - dis_to_ball) / 5000.0f;
    dis_score       = std::clamp(dis_score, 0.1f, 1.0f);

    // Calculate another score based on the robot's physical capabilities
    const auto &physical_status = Common::config().soccer.robot_physical_status[t_robot_idx];

    float caps_score = 1.0f;
    if (!physical_status.is_3D_printed)
        caps_score *= 0.8f;
    if (!physical_status.has_direct_kick && t_assignment.needs_shoot)
        caps_score *= 0.1f;
    if (!physical_status.has_chip_kick && t_assignment.needs_chip)
        caps_score *= 0.5f;

    // TODO: add a check for the robot's current role

    // Return the combined cost
    (void) mark_score;
    const int cost = 1000.0f * (2.f - dis_score * caps_score);
    return cost;
}
} // namespace Tyr::Soccer
