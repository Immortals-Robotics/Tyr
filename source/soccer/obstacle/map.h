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

    void addCircle(Common::Circle t_circle)
    {
        m_circle_obstacles.emplace_back(t_circle);

        Common::debug().draw(t_circle, Common::Color::maroon().transparent(), false);
    }

    void addRectangle(Common::Rect t_rect)
    {
        m_rect_obstacles.emplace_back(t_rect);

        Common::debug().draw(t_rect, Common::Color::maroon().transparent(), false);
    }

    bool  isInObstacle(Common::Vec2 t_point);
    float nearestDistance(Common::Vec2 t_point);

    bool collisionDetect(Common::Vec2 p1, Common::Vec2 p2);

    void resetMap()
    {
        m_circle_obstacles.clear();
        m_rect_obstacles.clear();
    }
};

extern ObstacleMap g_obs_map;

} // namespace Tyr::Soccer
