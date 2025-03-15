#pragma once

#include "circle.h"
#include "rect.h"

namespace Tyr::Soccer
{
class ObstacleMap
{
private:
    std::vector<CircleObstacle> m_circles;
    std::vector<RectObstacle>   m_rects;

public:
    ObstacleMap() = default;

    void add(Common::Circle t_circle)
    {
        m_circles.emplace_back(t_circle);

        Common::debug().draw(t_circle, Common::Color::maroon().transparent(), false);
    }

    void add(Common::Rect t_rect)
    {
        m_rects.emplace_back(t_rect);

        Common::debug().draw(t_rect, Common::Color::maroon().transparent(), false);
    }

    bool  inside(Common::Vec2 t_point) const;
    float distance(Common::Vec2 t_point) const;

    bool collisionDetect(Common::Vec2 p1, Common::Vec2 p2) const;

    void resetMap()
    {
        m_circles.clear();
        m_rects.clear();
    }
};

} // namespace Tyr::Soccer
