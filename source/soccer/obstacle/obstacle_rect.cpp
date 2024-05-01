#include "obstacle.h"

namespace Tyr::Soccer
{
bool RectangleObstacle::IsInObstacle(const Common::Vec2 t_point)
{
    return m_rect.inside(t_point);
}

float RectangleObstacle::NearestDistance(const Common::Vec2 t_point)
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
} // namespace Tyr::Soccer
