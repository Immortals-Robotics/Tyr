#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::gkRoleCost(const int t_robot_idx, const Assignment &t_assignment) const
{
    (void) t_assignment;
    return m_ref_state.ourInfo().gk_id == t_robot_idx ? 0.0f : -1.0f;
}
} // namespace Tyr::Soccer
