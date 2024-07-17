#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::createAttackAssignment()
{
    Assignment assignment;
    assignment.priority      = Assignment::Priority::High;
    assignment.needs_shoot   = true;
    assignment.needs_chip    = true;
    assignment.cost_function = std::bind(&Ai::attackRoleCost, this, std::placeholders::_1, std::placeholders::_2);
    assignment.role          = &m_attack;

    m_assignments.push_back(assignment);
}
} // namespace Tyr::Soccer
