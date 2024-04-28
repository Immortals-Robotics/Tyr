#pragma once
#include "../vector.h"

namespace Tyr::Common
{
struct Rect
{
public:
    Rect(const Vec2 t_p1, const Vec2 t_p2)
    {
        min_x = std::min(t_p1.x, t_p2.x);
        min_y = std::min(t_p1.y, t_p2.y);
        max_x = std::max(t_p1.x, t_p2.x);
        max_y = std::max(t_p1.y, t_p2.y);
    }

    bool inside(Vec2 t_point) const;

    bool inside_offset(const Vec2 t_point, float offset) const;

    Vec2 nearestOutside(Vec2 t_point) const;

    bool isCircleCross(Vec2 t_point1, Vec2 t_point2, float t_r) const;

    float min_x = 0.0;
    float min_y = 0.0;
    float max_x = 0.0;
    float max_y = 0.0;
};
} // namespace Tyr::Common
