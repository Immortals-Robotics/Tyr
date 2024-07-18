#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::createDefAssignments()
{
    createStaticAssignment(&m_def1, Assignment::Priority::Max);
    createStaticAssignment(&m_def2, Assignment::Priority::Max);
}
} // namespace Tyr::Soccer
