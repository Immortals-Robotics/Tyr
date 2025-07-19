#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::createDefAssignments()
{
#if 1
    createStaticAssignment(&m_def1, Assignment::Priority::Low);
    createStaticAssignment(&m_def2, Assignment::Priority::Low);
#else
    createStaticAssignment(&m_def1, Assignment::Priority::Max);
    createStaticAssignment(&m_def2, Assignment::Priority::Max);
#endif
}
} // namespace Tyr::Soccer
