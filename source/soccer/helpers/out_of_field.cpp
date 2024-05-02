#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::outOfField(Common::Vec2 point)
{
    float THR = 0.0; // 200.0;
    if (std::fabs(point.x) > Common::worldState().field.width || std::fabs(point.y) > Common::worldState().field.height)
    {
        return true;
    }
    return false;
}
} // namespace Tyr::Soccer
