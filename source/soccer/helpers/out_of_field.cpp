#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::outOfField(Common::Vec2 point)
{
    float THR = 0.0; // 200.0;
    if (std::fabs(point.x) > field_width || std::fabs(point.y) > field_height)
    {
        return true;
    }
    return false;
}
} // namespace Tyr::Soccer
