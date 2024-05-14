#pragma once

#include "circle.h"
#include "rect.h"

namespace Tyr::Soccer
{
class ObstacleMap
{
private:
    std::vector<CircleObstacle> m_circle_obstacles;
    std::vector<RectObstacle>   m_rect_obstacles;

public:
    ObstacleMap() = default;

    inline void addCircle(Common::Circle t_circle)
    {
        // TODO: verify if this check is needed
        if (t_circle.r > 0)
        {
            m_circle_obstacles.emplace_back(t_circle);
        }
    }

    inline void addRectangle(Common::Rect t_rect)
    {
        m_rect_obstacles.emplace_back(t_rect);
    }

    bool  isInObstacle(Common::Vec2 t_point);
    float nearestDistance(Common::Vec2 t_point);

    bool collisionDetect(const Common::Vec2 p1, const Common::Vec2 p2);

    inline void resetMap()
    {
        m_circle_obstacles.clear();
        m_rect_obstacles.clear();
    }
};

extern ObstacleMap g_obs_map;

} // namespace Tyr::Soccer
