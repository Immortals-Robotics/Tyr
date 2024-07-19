#include "../ai.h"

namespace Tyr::Soccer
{
int Ai::staticRoleCost(const int t_robot_idx, const Assignment &t_assignment) const
{
    const int dis_cost = m_world_state.own_robot[t_robot_idx].position.distanceTo(t_assignment.target_point);

    const auto &physical_status = Common::config().soccer.robot_physical_status[t_robot_idx];

    int caps_cost = 0;
    if (t_assignment.needs_shoot != physical_status.has_direct_kick)
        caps_cost += 5000;
    if (t_assignment.needs_chip != physical_status.has_chip_kick)
        caps_cost += 5000;

    int switch_cost = t_robot_idx == *t_assignment.role ? 0 : 500;

    return dis_cost + caps_cost + switch_cost;
}
} // namespace Tyr::Soccer
