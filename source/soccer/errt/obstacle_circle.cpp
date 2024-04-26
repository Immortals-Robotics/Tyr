#include "obstacle_new.h"

namespace Tyr::Soccer
{
CircleObstacle::CircleObstacle(float _x, float _y, float _r)
{
    x = _x;
    y = _y;
    r = _r;
}

bool CircleObstacle::IsInObstacle(float _x, float _y)
{
    if ((_x > x + r) || (_x < x - r) || (_y > y + r) || (_y < y - r))
        return false;

    if (Common::distance(Common::vec2(x, y), Common::vec2(_x, _y)) <= r)
        return true;

    return false;
}

float CircleObstacle::NearestDistance(float _x, float _y)
{
    return Common::distance(Common::vec2(x, y), Common::vec2(_x, _y)) - r;
}
} // namespace Tyr::Soccer
