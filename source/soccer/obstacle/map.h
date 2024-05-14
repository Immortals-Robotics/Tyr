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
    ObstacleMap();

    void addCircle(Common::Circle t_circle);
    void addRectangle(Common::Rect t_rect);

    bool  isInObstacle(Common::Vec2 t_point);
    float nearestDistance(Common::Vec2 t_point);

    bool collisionDetect(const Common::Vec2 p1, const Common::Vec2 p2);

    void resetMap();
};

extern ObstacleMap obs_map;

} // namespace Tyr::Soccer
