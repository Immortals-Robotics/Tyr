#pragma once

#include "../vector.h"

namespace Tyr::Common
{
struct Rect
{
public:
    Rect() = default;

    Rect(const Vec2 t_p1, const Vec2 t_p2)
        : min(std::min(t_p1.x, t_p2.x), std::min(t_p1.y, t_p2.y)),
          max(std::max(t_p1.x, t_p2.x), std::max(t_p1.y, t_p2.y))
    {}

    Rect(const Vec2 t_p, const float t_w, const float t_h) : Rect(t_p, t_p + Vec2(t_w, t_h))
    {}

    Rect(const Protos::Immortals::Rect &t_rect) : Rect(t_rect.min(), t_rect.max())
    {}

    void fillProto(Protos::Immortals::Rect *const t_rect) const
    {
        min.fillProto(t_rect->mutable_min());
        max.fillProto(t_rect->mutable_max());
    }

    inline bool inside(Vec2 t_point) const
    {
        return min.x <= t_point.x && max.x >= t_point.x && min.y <= t_point.y && max.y >= t_point.y;
    }

    inline bool insideOffset(const Vec2 t_point, float offset) const
    {
        return (min.x - offset) <= t_point.x && (max.x + offset) >= t_point.x && (min.y - offset) <= t_point.y &&
               (max.y + offset) >= t_point.y;
    }

    inline Vec2 nearestOutside(Vec2 t_point) const
    {
        const float extension = 5.0f;

        float deltaXmin = abs(t_point.x - min.x);
        float deltaXmax = abs(t_point.x - max.x);
        float deltaYmin = abs(t_point.y - min.y);
        float deltaYmax = abs(t_point.y - max.y);

        float min_xy = std::min(std::min(deltaXmin, deltaXmax), std::min(deltaYmin, deltaYmax));

        if (deltaXmin == min_xy)
        {
            return Vec2(min.x - extension, t_point.y);
        }
        else if (deltaXmax == min_xy)
        {
            return Vec2(max.x + extension, t_point.y);
        }
        else if (deltaYmin == min_xy)
        {
            return Vec2(t_point.x, min.y - extension);
        }
        else
        {
            //(deltaYmax == min_xy)
            return Vec2(t_point.x, max.y + extension);
        }
    }

    inline bool isCircleCross(Vec2 t_point1, Vec2 t_point2, float t_r) const
    {
        return (min.x <= t_point1.x) && (max.x >= t_point1.x) && (min.y <= t_point1.y) && (max.y >= t_point1.y);
    }

    Vec2 min;
    Vec2 max;
};
} // namespace Tyr::Common
