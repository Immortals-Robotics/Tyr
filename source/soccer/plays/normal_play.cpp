#include "../ai.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer_omid;

void Ai::newNormalPlay()
{
    gkHi(m_gk);

    defHi(m_def1, m_def2, nullptr);

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
