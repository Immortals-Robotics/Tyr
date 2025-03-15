#pragma once

namespace Tyr::Soccer
{
class CircleObstacle
{
private:
    Common::Circle m_circle;

public:
    CircleObstacle(const Common::Circle t_circle) : m_circle(t_circle)
    {}

    bool inside(const Common::Vec2 t_point, const float t_margin = 0.f) const
    {
        return m_circle.inside(t_point, t_margin);
    }

    float distance(const Common::Vec2 t_point) const
    {
        return m_circle.distance(t_point);
    }
};
} // namespace Tyr::Soccer
