#include "vector.h"

#include "angle.h"

namespace Tyr::Common
{
vec2 circle_around_point(vec2 point, Angle angle, float radius)
{
    return point + angle.toUnitVec() * radius;
}

vec2 point_on_connecting_line(vec2 first_point, vec2 second_point, float distance)
{
    float m = (second_point.y - first_point.y) / (second_point.x - first_point.x);
    vec2  ans;
    if (second_point.x - first_point.x > 0)
        ans.x = first_point.x + distance / std::sqrt(std::pow(m, 2.0f) + 1.0f);
    else
        ans.x = first_point.x - distance / std::sqrt(std::pow(m, 2.0f) + 1.0f);
    ans.y = first_point.y + m * (ans.x - first_point.x);
    return ans;
}

} // namespace Tyr::Common