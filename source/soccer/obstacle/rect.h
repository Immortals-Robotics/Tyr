#pragma once

namespace Tyr::Soccer
{
class RectObstacle
{
private:
    Common::Rect m_rect;

public:
    RectObstacle(const Common::Rect t_rect) : m_rect(t_rect)
    {}

    inline bool IsInObstacle(Common::Vec2 t_point) const
    {
        return m_rect.inside(t_point);
    }

    inline float NearestDistance(Common::Vec2 t_point) const
    {
        if (IsInObstacle(t_point))
            return -1.0f;

        if ((t_point.x > m_rect.min.x) && (t_point.x < m_rect.max.x))
        {
            return std::min(std::fabs(t_point.y - m_rect.min.y), std::fabs(t_point.y - m_rect.max.y));
        }

        if ((t_point.y > m_rect.min.y) && (t_point.y < m_rect.max.y))
        {
            return std::min(std::fabs(t_point.x - m_rect.min.x), std::fabs(t_point.x - m_rect.max.x));
        }

        if ((t_point.x < m_rect.min.x) && (t_point.y < m_rect.min.y))
        {
            return t_point.distanceTo(m_rect.min);
        }

        if ((t_point.x < m_rect.min.x) && (t_point.y > m_rect.max.y))
        {
            return t_point.distanceTo(Common::Vec2(m_rect.min.x, m_rect.max.y));
        }

        if ((t_point.x > m_rect.max.x) && (t_point.y < m_rect.min.y))
        {
            return t_point.distanceTo(Common::Vec2(m_rect.max.x, m_rect.min.y));
        }

        if ((t_point.x > m_rect.max.x) && (t_point.y > m_rect.max.y))
        {
            return t_point.distanceTo(Common::Vec2(m_rect.max.x, m_rect.max.y));
        }

        return 0.0f;
    }
};
} // namespace Tyr::Soccer
