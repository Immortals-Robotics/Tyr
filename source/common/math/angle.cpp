#include "angle.h"

namespace Tyr::Common
{
Vec2 Angle::toUnitVec() const
{
    const float rad = this->rad();
    return {std::cos(rad), std::sin(rad)};
}

Angle Angle::average(const Angle a, const Angle b)
{
    const Vec2 average_vector = (a.toUnitVec() + b.toUnitVec()) / 2.0f;
    return average_vector.toAngle();
}
} // namespace Tyr::Common
