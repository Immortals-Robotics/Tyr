#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::CalcIsDefending()
{
    if (isDefending)
    {
        if (side * ball.Position.x < -200)
        {
            isDefending = false;
        }
    }
    else if (side * ball.Position.x > 300)
    {
        isDefending = true;
    }

    if (oppRestarted && timer.time() < 2.0)
    {
        isDefending = true;
    }
}
} // namespace Tyr::Soccer
