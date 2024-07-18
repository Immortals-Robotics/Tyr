#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::createStaticAssignment(int *const t_role, const Assignment::Priority t_priority, const bool t_shoot, const bool t_chip)
{
    const Robot &robot = m_own_robot[*t_role];

    Assignment assignment;
    assignment.priority      = t_priority;
    assignment.cost_function = std::bind(&Ai::staticRoleCost, this, std::placeholders::_1, std::placeholders::_2);
    assignment.role          = t_role;
    assignment.target_point  = robot.target.position;
    assignment.needs_shoot   = t_shoot;
    assignment.needs_chip    = t_chip;

    m_assignments.push_back(assignment);
}
} // namespace Tyr::Soccer
