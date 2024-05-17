#include "../ai.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer_omid;

void Ai::newNormalPlay()
{
    gkHi(m_gk, 0);

    defHi(m_def, m_rw, m_lw, nullptr, false);

    calcIsDefending();

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
