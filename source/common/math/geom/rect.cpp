#include "rect.h"

namespace Tyr::Common
{
bool Rect::inside(const Vec2 t_point) const
{
    return min_x <= t_point.x && max_x >= t_point.x && min_y <= t_point.y && max_y >= t_point.y;
}

bool Rect::insideOffset(const Vec2 t_point, float offset) const
{
    return (min_x - offset) <= t_point.x && (max_x + offset) >= t_point.x && (min_y - offset) <= t_point.y &&
           (max_y + offset) >= t_point.y;
}

Vec2 Rect::nearestOutside(const Vec2 t_point) const
{
    float thr       = 5;
    float deltaXmin = abs(t_point.x - min_x);
    float deltaXmax = abs(t_point.x - max_x);
    float deltaYmin = abs(t_point.y - min_y);
    float deltaYmax = abs(t_point.y - max_y);

    float min_xy = std::min(std::min(deltaXmin, deltaXmax), std::min(deltaYmin, deltaYmax));

    if (deltaXmin == min_xy)
    {
        return Vec2(min_x - thr, t_point.y);
    }
    else if (deltaXmax == min_xy)
    {
        return Vec2(max_x + thr, t_point.y);
    }
    else if (deltaYmin == min_xy)
    {
        return Vec2(t_point.x, min_y - thr);
    }
    else
    {
        //(deltaYmax == min_xy)
        return Vec2(t_point.x, max_y + thr);
    }
}

bool Rect::isCircleCross(const Vec2 t_point1, Vec2 t_point2, float t_r) const
{
    return (min_x <= t_point1.x) && (max_x >= t_point1.x) && (min_y <= t_point1.y) && (max_y >= t_point1.y);
}
} // namespace Tyr::Common
