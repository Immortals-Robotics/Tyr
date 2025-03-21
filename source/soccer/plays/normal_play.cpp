#include "../ai.h"
#include "../tactics/gk.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer_omid;

void Ai::newNormalPlay()
{
    calcIsDefending();

    m_assignments.clear();
    createGkAssignment();
    createDefAssignments();
    createMidAssignments();
    createAttackAssignment();
    assignRoles();

    GkTactic{}.execute(m_own_robot[m_gk]);

    defHi(m_own_robot[m_def1], m_own_robot[m_def2], nullptr);

    if (m_is_defending)
    {
        normalPlayDef();
    }
    else
    {
        normalPlayAtt();
    }
}
} // namespace Tyr::Soccer
