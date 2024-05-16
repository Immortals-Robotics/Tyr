#include "map.h"

namespace Tyr::Soccer
{
ObstacleMap g_obs_map{};

bool ObstacleMap::isInObstacle(const Common::Vec2 t_p)
{
    const float field_margin = Common::field().boundary_width - Common::field().robot_radius;
    if (std::fabs(t_p.x) > Common::field().width + field_margin ||
        std::fabs(t_p.y) > Common::field().height + field_margin)
    {
        return true;
    }

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

    return false;
}

float ObstacleMap::nearestDistance(const Common::Vec2 t_p)
{
    float dis = std::numeric_limits<float>::max();

    for (const auto &obstacle : m_circle_obstacles)
    {
        const float tmp_dis = obstacle.NearestDistance(t_p);
        if (tmp_dis < dis)
            dis = tmp_dis;
        if (dis <= 0)
            return dis;
    }

    for (const auto &obstacle : m_rect_obstacles)
    {
        const float tmp_dis = obstacle.NearestDistance(t_p);
        if (tmp_dis < dis)
            dis = tmp_dis;
        if (dis <= 0)
            return dis;
    }

    return dis;
}

bool ObstacleMap::collisionDetect(const Common::Vec2 p1, const Common::Vec2 p2)
{
    static constexpr float kStepSize = 50.0f;

    const Common::Vec2 step = (p2 - p1).normalized() * kStepSize;

    Common::Vec2 current = p1;

    while (current.distanceTo(p2) > kStepSize)
    {
        if (isInObstacle(current))
            return true;

        current += step;
    }

    return false;
}
} // namespace Tyr::Soccer
