#pragma once

namespace Immortals::Common
{
inline bool almostEqual(const float t_a, const float t_b)
{
    return std::abs(t_a - t_b) <= std::numeric_limits<float>::epsilon();
}

inline float sign(const float t_f)
{
    return std::copysign(1.0f, t_f);
}

inline int signInt(const float t_f)
{
    return std::copysign(1, t_f);
}
} // namespace Immortals::Common
