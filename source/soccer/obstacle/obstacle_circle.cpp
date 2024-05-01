#include "obstacle_new.h"

namespace Tyr::Soccer
{
bool CircleObstacle::IsInObstacle(const Common::Vec2 t_point)
{
    return m_circle.inside(t_point);
}

float CircleObstacle::NearestDistance(const Common::Vec2 t_point)
{
    return m_circle.center.distanceTo(t_point) - m_circle.r;
}
} // namespace Tyr::Soccer
