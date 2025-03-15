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

    bool inside(const Common::Vec2 t_point, const float t_margin = 0.f) const
    {
        return m_rect.inside(t_point, t_margin);
    }

    float distance(const Common::Vec2 t_point) const
    {
        return m_rect.distance(t_point);
    }
};
} // namespace Tyr::Soccer
