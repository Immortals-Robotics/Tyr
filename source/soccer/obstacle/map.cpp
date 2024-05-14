#include "map.h"

namespace Tyr::Soccer
{
ObstacleMap g_obs_map{};

bool ObstacleMap::isInObstacle(const Common::Vec2 t_p)
{
    for (const auto &obstacle : m_circle_obstacles)
    {
        if (obstacle.IsInObstacle(t_p))
            return true;
    }

    for (const auto &obstacle : m_rect_obstacles)
    {
        if (obstacle.IsInObstacle(t_p))
            return true;
    }

    if (std::fabs(t_p.x) > 10000 ||
        std::fabs(t_p.y) > 10000) // If the  values where getting to much (this fixes the errors for now...
    {
        return true;
    }
    return false;
}

float ObstacleMap::nearestDistance(const Common::Vec2 t_p)
{
    float dis = std::numeric_limits<float>::max();
    float tmp_dis;

    for (const auto &obstacle : m_circle_obstacles)
    {
        tmp_dis = obstacle.NearestDistance(t_p);
        if (tmp_dis < dis)
            dis = tmp_dis;
        if (dis <= 0)
            return dis;
    }

    for (const auto &obstacle : m_rect_obstacles)
    {
        tmp_dis = obstacle.NearestDistance(t_p);
        if (tmp_dis < dis)
            dis = tmp_dis;
        if (dis <= 0)
            return dis;
    }

    return dis;
}

bool ObstacleMap::collisionDetect(const Common::Vec2 p1, const Common::Vec2 p2)
{
    float coss, sinn;
    coss = (p2.x - p1.x) / p1.distanceTo(p2);
    sinn = (p2.y - p1.y) / p1.distanceTo(p2);

    Common::Vec2 current = p1;

    while (current.distanceTo(p2) > 10.0f)
    {
        if (isInObstacle(current))
            return true;

        current.x += coss * 10.0f;
        current.y += sinn * 10.0f;
    }

    return false;
}
} // namespace Tyr::Soccer
