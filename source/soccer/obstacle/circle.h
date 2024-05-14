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

    inline bool IsInObstacle(Common::Vec2 t_point) const
    {
        return m_circle.inside(t_point);
    }

    inline float NearestDistance(Common::Vec2 t_point) const
    {
        return m_circle.center.distanceTo(t_point) - m_circle.r;
    }
};
} // namespace Tyr::Soccer
