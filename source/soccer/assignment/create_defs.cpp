#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::createDefAssignments()
{
    {
        Assignment assignment;
        assignment.priority      = Assignment::Priority::Max;
        assignment.cost_function = std::bind(&Ai::staticRoleCost, this, std::placeholders::_1, std::placeholders::_2);
        assignment.role          = &m_def1;
        // TODO: verify if this is correct
        assignment.target_point = m_own_robot[m_def1].target.position;

        m_assignments.push_back(assignment);
    }
    {
        Assignment assignment;
        assignment.priority      = Assignment::Priority::Max;
        assignment.cost_function = std::bind(&Ai::staticRoleCost, this, std::placeholders::_1, std::placeholders::_2);
        assignment.role          = &m_def2;
        // TODO: verify if this is correct
        assignment.target_point = m_own_robot[m_def2].target.position;

        m_assignments.push_back(assignment);
    }
}
} // namespace Tyr::Soccer
