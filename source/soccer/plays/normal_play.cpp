#include "../ai.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer_omid;

void Ai::NewNormalPlay()
{
    GKHi(gk, 0);

    DefBy3(def, rw, lw, nullptr, false);

    CalcIsDefending();

    if (isDefending)
    {
        NormalPlayDef();
    }
    else
    {
        NormalPlayAtt();
    }
}
} // namespace Tyr::Soccer
