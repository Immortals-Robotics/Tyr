#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::outOfField(Common::Vec2 point)
{
    if (std::fabs(point.x) > Common::field().width || std::fabs(point.y) > Common::field().height)
    {
        return true;
    }
    return false;
}
} // namespace Tyr::Soccer
