#pragma once

namespace Tyr::Common
{
inline bool almostEqual(const float t_a, const float t_b)
{
    return std::abs(t_a - t_b) <= std::numeric_limits<float>::epsilon();
}

inline float sign(const float t_f)
{
    return std::copysign(1.0f, t_f);
}

float average_deque(std::deque<float> _q);
float variance_deque(std::deque<float> _q);

// This method performs the abc formula (Pythagoras' Theorem) on the given parameters.
std::vector<float> abcFormula(float t_a, float t_b, float t_c);
} // namespace Tyr::Common
