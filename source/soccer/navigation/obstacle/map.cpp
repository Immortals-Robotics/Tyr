#include "map.h"

namespace Tyr::Soccer
{
bool ObstacleMap::inside(const Common::Vec2 t_p, const float t_margin) const
{
    // TODO: make this an actual obstacle
    const float margin = Common::field().boundary_width - Common::field().robot_radius + 10.f;
    if (std::fabs(t_p.x) > Common::field().width + margin ||
        std::fabs(t_p.y) > Common::field().height + margin)
    {
        return true;
    }

    for (const auto &obstacle : m_circles)
    {
        if (!(obstacle.physicality() & physicality()))
            continue;

        if (obstacle.inside(t_p, t_margin))
            return true;
    }

    for (const auto &obstacle : m_rects)
    {
        if (!(obstacle.physicality() & physicality()))
            continue;

        if (obstacle.inside(t_p, t_margin))
            return true;
    }

    return false;
}

float ObstacleMap::distance(const Common::Vec2 t_p) const
{
    float dis = std::numeric_limits<float>::max();

    for (const auto &obstacle : m_circles)
    {
        if (!(obstacle.physicality() & physicality()))
            continue;

        const float tmp_dis = obstacle.distance(t_p);
        if (tmp_dis < dis)
            dis = tmp_dis;
        if (dis <= 0)
            return dis;
    }

    for (const auto &obstacle : m_rects)
    {
        if (!(obstacle.physicality() & physicality()))
            continue;

        const float tmp_dis = obstacle.distance(t_p);
        if (tmp_dis < dis)
            dis = tmp_dis;
        if (dis <= 0)
            return dis;
    }

    return dis;
}

bool ObstacleMap::collisionDetect(const Common::Vec2 p1, const Common::Vec2 p2) const
{
    static constexpr float kStepSize = 50.0f;

    const Common::Vec2 step = (p2 - p1).normalized() * kStepSize;

    Common::Vec2 current = p1;

    while (current.distanceTo(p2) > kStepSize)
    {
        if (inside(current))
            return true;

        current += step;
    }

    return false;
}
} // namespace Tyr::Soccer
