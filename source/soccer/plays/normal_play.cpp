#include "../ai.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer_omid;

void Ai::newNormalPlay()
{
    gkHi(gk, 0);

    defHi(def, rw, lw, nullptr, false);

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
