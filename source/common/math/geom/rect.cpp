#include "rect.h"

namespace Tyr::Common
{
bool Rect::inside(const Vec2 t_point) const
{
    return min.x <= t_point.x && max.x >= t_point.x && min.y <= t_point.y && max.y >= t_point.y;
}

bool Rect::insideOffset(const Vec2 t_point, float offset) const
{
    return (min.x - offset) <= t_point.x && (max.x + offset) >= t_point.x && (min.y - offset) <= t_point.y &&
           (max.y + offset) >= t_point.y;
}

Vec2 Rect::nearestOutside(const Vec2 t_point) const
{
    float thr       = 5;
    float deltaXmin = abs(t_point.x - min.x);
    float deltaXmax = abs(t_point.x - max.x);
    float deltaYmin = abs(t_point.y - min.y);
    float deltaYmax = abs(t_point.y - max.y);

    float min_xy = std::min(std::min(deltaXmin, deltaXmax), std::min(deltaYmin, deltaYmax));

    if (deltaXmin == min_xy)
    {
        return Vec2(min.x - thr, t_point.y);
    }
    else if (deltaXmax == min_xy)
    {
        return Vec2(max.x + thr, t_point.y);
    }
    else if (deltaYmin == min_xy)
    {
        return Vec2(t_point.x, min.y - thr);
    }
    else
    {
        //(deltaYmax == min_xy)
        return Vec2(t_point.x, max.y + thr);
    }
}

bool Rect::isCircleCross(const Vec2 t_point1, Vec2 t_point2, float t_r) const
{
    return (min.x <= t_point1.x) && (max.x >= t_point1.x) && (min.y <= t_point1.y) && (max.y >= t_point1.y);
}
} // namespace Tyr::Common
