#include "helpers.h"

namespace Tyr::Common
{

bool almostEqual(const float t_a, const float t_b)
{
    return std::abs(t_a - t_b) <= std::numeric_limits<float>::epsilon();
}

float sign(const float t_f)
{
    return std::copysign(1.0f, t_f);
}

float average_deque(std::deque<float> _q)
{
    float tmp = 0.0;
    for (float elem : _q)
    {
        tmp += elem;
    }
    tmp /= _q.size();
    return tmp;
}

float variance_deque(std::deque<float> _q)
{
    float avg = average_deque(_q);
    float sum = 0.0;
    for (float elem : _q)
    {
        sum += (elem - avg) * (elem - avg);
    }

    return sum / _q.size();
}

std::vector<float> abcFormula(const float t_a, const float t_b, const float t_c)
{
    // discriminant is b^2 - 4*a*c
    const float discr = t_b * t_b - 4 * t_a * t_c;

    if (std::abs(discr) < std::numeric_limits<float>::epsilon())
    {
        // discriminant = 0: // only one solution
        return {-t_b / (2 * t_a)};
    }
    else if (discr < 0)
    {
        // discriminant < 0: no solutions
        return {};
    }
    else
    {
        // discriminant > 0: two solutions
        const float discr_sq = std::sqrt(discr);
        return {(-t_b + discr_sq) / (2 * t_a), (-t_b - discr_sq) / (2 * t_a)};
    }
}
} // namespace Tyr::Common
