#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::outOfField(Common::Vec2 point)
{
    return std::fabs(point.x) > Common::field().width || std::fabs(point.y) > Common::field().height;
}
} // namespace Tyr::Soccer
