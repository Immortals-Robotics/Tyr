#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::createGkAssignment()
{
    Assignment assignment;
    assignment.priority      = Assignment::Priority::Max;
    assignment.needs_chip    = true;
    assignment.cost_function = std::bind(&Ai::gkRoleCost, this, std::placeholders::_1, std::placeholders::_2);
    assignment.role          = &m_gk;

    m_assignments.push_back(assignment);
}
} // namespace Tyr::Soccer
