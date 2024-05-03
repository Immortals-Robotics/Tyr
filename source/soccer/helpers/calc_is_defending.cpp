#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::CalcIsDefending()
{
    if (isDefending)
    {
        if (side * Common::worldState().ball.position.x < -200)
        {
            isDefending = false;
        }
    }
    else if (side * Common::worldState().ball.position.x > 300)
    {
        isDefending = true;
    }

    if (oppRestarted && timer.time() < 2.0)
    {
        isDefending = true;
    }
}
} // namespace Tyr::Soccer
