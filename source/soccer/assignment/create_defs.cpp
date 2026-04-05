#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::createDefAssignments()
{
#if 0
    createStaticAssignment(&m_def1, Assignment::Priority::Low);
    createStaticAssignment(&m_def2, Assignment::Priority::Low);
#else
    createStaticAssignment(&m_def1, Assignment::Priority::High);
    createStaticAssignment(&m_def2, Assignment::Priority::High);
#endif
}
} // namespace Tyr::Soccer
