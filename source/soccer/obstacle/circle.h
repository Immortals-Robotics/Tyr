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

    bool IsInObstacle(const Common::Vec2 t_point) const
    {
        return m_circle.inside(t_point);
    }

    float NearestDistance(const Common::Vec2 t_point) const
    {
        return m_circle.center.distanceTo(t_point) - m_circle.r;
    }
};
} // namespace Tyr::Soccer
