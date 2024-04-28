#include "obstacle_new.h"

namespace Tyr::Soccer
{
RectangleObstacle::RectangleObstacle(float _x, float _y, float _w, float _h)
{
    if (_w < 0)
    {
        x = _x + _w;
        w = -_w;
    }
    else
    {
        x = _x;
        w = _w;
    }

    if (_h < 0)
    {
        y = _y + _h;
        h = _h;
    }
    else
    {
        y = _y;
        h = _h;
    }
}

bool RectangleObstacle::IsInObstacle(float _x, float _y)
{
    return (_x < x + w) && (_x > x) && (_y < y + h) && (_y > y);
}

float RectangleObstacle::NearestDistance(float _x, float _y)
{
    if (IsInObstacle(_x, _y))
        return -1.0f;

    if ((_x > x) && (_x < x + w))
    {
        return std::min(fabs(_y - y), fabs(_y - y - h));
    }

    if ((_y > y) && (_y < y + h))
    {
        return std::min(fabs(_x - x), fabs(_x - x - w));
    }

    if ((_x < x) && (_y < y))
    {
        return Common::Vec2(_x, _y).distanceTo(Common::Vec2(x, y));
    }

    if ((_x < x) && (_y > y + h))
    {
        return Common::Vec2(_x, _y).distanceTo(Common::Vec2(x, y + h));
    }

    if ((_x > x + w) && (_y < y))
    {
        return Common::Vec2(_x, _y).distanceTo(Common::Vec2(x + w, y));
    }

    if ((_x > x + w) && (_y > y + h))
    {
        return Common::Vec2(_x, _y).distanceTo(Common::Vec2(x + w, y + h));
    }

    return 0;
}
} // namespace Tyr::Soccer
