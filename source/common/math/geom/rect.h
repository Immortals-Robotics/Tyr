#pragma once
#include "../vector.h"

namespace Tyr::Common
{
struct Rect
{
public:
    Rect(const Vec2 t_p1, const Vec2 t_p2)
        : min(std::min(t_p1.x, t_p2.x), std::min(t_p1.y, t_p2.y)),
          max(std::max(t_p1.x, t_p2.x), std::max(t_p1.y, t_p2.y))
    {}

    bool inside(Vec2 t_point) const;

    bool insideOffset(const Vec2 t_point, float offset) const;

    Vec2 nearestOutside(Vec2 t_point) const;

    bool isCircleCross(Vec2 t_point1, Vec2 t_point2, float t_r) const;

    Vec2 min;
    Vec2 max;
};
} // namespace Tyr::Common
