#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::staticRoleCost(const int t_robot_idx, const Assignment &t_assignment) const
{
    // TODO: take shoot and chip requirements into account
    return m_world_state.own_robot[t_robot_idx].position.distanceTo(t_assignment.target_point);
}
} // namespace Tyr::Soccer
