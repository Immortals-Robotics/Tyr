#include "../ai.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer_omid;

void Ai::NewNormalPlay()
{
    //	GKHi(gk, 0);
    // side = -side;
    GKHi(gk, 0);
    // side = -side;//TODO #8 comment this ...

    //	DefHi(def,nullptr, 0);
    //	side = -side;
    //    DefHi(def,nullptr, 0);
    DefMid(def, rw, lw, nullptr, false);
    //	side = -side;

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
