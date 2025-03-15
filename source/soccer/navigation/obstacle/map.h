#pragma once

#include "circle.h"
#include "rect.h"
#include "inverse_rect.h"

namespace Tyr::Soccer
{
class ObstacleMap
{
private:
    std::vector<CircleObstacle> m_circles;
    std::vector<RectObstacle>   m_rects;
    std::vector<InverseRectObstacle> m_inverse_rects;

public:
    ObstacleMap() = default;

    void add(CircleObstacle t_circle)
    {
        m_circles.emplace_back(t_circle);

        //Common::debug().draw(t_circle, Common::Color::maroon().transparent(), false);
    }

    void add(RectObstacle t_rect)
    {
        m_rects.emplace_back(t_rect);

        //Common::debug().draw(t_rect, Common::Color::maroon().transparent(), false);
    }

    void add(InverseRectObstacle t_rect)
    {
        m_inverse_rects.emplace_back(t_rect);

        //Common::debug().draw(t_rect, Common::Color::maroon().transparent(), false);
    }

    bool inside(Common::Vec2 t_point, const float t_margin = 0.f) const
    {
        return distance(t_point) < t_margin;
    }

    float distance(const Common::Vec2 t_point) const
    {
        float min_dis = std::numeric_limits<float>::max();

        for (const auto &obstacle : m_circles)
        {
            const float dis = obstacle.distance(t_point);
            if (dis < min_dis)
                min_dis = dis;
        }

        for (const auto &obstacle : m_rects)
        {
            const float dis = obstacle.distance(t_point);
            if (dis < min_dis)
                min_dis = dis;
        }

        for (const auto &obstacle : m_inverse_rects)
        {
            const float dis = obstacle.distance(t_point);
            if (dis < min_dis)
                min_dis = dis;
        }

        return min_dis;
    }

    bool collisionDetect(const Common::LineSegment t_line) const
    {
        static constexpr float kStepSize = 50.0f;

        const Common::Vec2 step = (t_line.end - t_line.start).normalized() * kStepSize;

        Common::Vec2 current = t_line.start;

        while (current.distanceTo(t_line.end) > kStepSize)
        {
            if (inside(current))
                return true;

            current += step;
        }

        return false;
    }

    void resetMap()
    {
        m_circles.clear();
        m_rects.clear();
        m_inverse_rects.clear();
    }
};

} // namespace Tyr::Soccer
