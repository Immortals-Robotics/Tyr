#include "../ai.h"

namespace Tyr::Soccer
{
int Ai::gkRoleCost(const int t_robot_idx, const Assignment &t_assignment) const
{
    (void) t_assignment;
    return static_cast<int>(m_ref_state.ourInfo().gk_id) == t_robot_idx ? 0 : -1;
}
} // namespace Tyr::Soccer
