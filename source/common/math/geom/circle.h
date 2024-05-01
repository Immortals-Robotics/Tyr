#pragma once

#include "../vector.h"

namespace Tyr::Common
{
struct Circle
{
public:
    Circle(const Vec2 t_center, const float t_r) : center(t_center), r(t_r)
    {}

    float circumference() const
    {
        return 2.0 * std::numbers::pi * r;
    }

    float area() const
    {
        return std::numbers::pi * r * r;
    }

    bool inside(const Vec2 t_point) const
    {
        return (center - t_point).length() < r;
    }

    bool insideOffset(const Vec2 t_point, float offset) const
    {
        return (center - t_point).length() < (r + offset);
    }

    Vec2 nearestOutside(Vec2 t_point, float t_extra_r = 0.0) const;

    // circle intersection
    std::vector<Vec2> intersect(const Circle &t_other) const;
    float             intersectionArea(const Circle &t_other) const;

    bool isCircleCross(Vec2 t_point1, Vec2 t_point2, float t_r) const;

    Vec2  center;
    float r;
};
} // namespace Tyr::Common
