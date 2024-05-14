#include "map.h"

#include "circle.h"
#include "rectangle.h"

namespace Tyr::Soccer
{
ObstacleMap obs_map{};

ObstacleMap::ObstacleMap()
{}

void ObstacleMap::addCircle(const Common::Circle t_circle)
{
    // TODO: verify if this check is needed
    if (t_circle.r > 0)
    {
        m_obstacles.emplace_back(std::make_unique<CircleObstacle>(t_circle));
    }
}
void ObstacleMap::addRectangle(const Common::Rect t_rect)
{
    m_obstacles.emplace_back(std::make_unique<RectangleObstacle>(t_rect));
}

bool ObstacleMap::isInObstacle(const Common::Vec2 t_p)
{
    for (int i = 0; i < getObsNum(); i++)
    {
        if (m_obstacles[i]->IsInObstacle(t_p))
            return true;
    }
    if (std::fabs(t_p.x) > 10000 ||
        std::fabs(t_p.y) > 10000) // If the  values where getting to much (this fixes the errors for now...
    {
        return true;
    }
    return false;
}

float ObstacleMap::nearestDistance(const Common::Vec2 t_p)
{
    float dis = std::numeric_limits<float>::max();
    float tmp_dis;
    for (int i = 0; i < getObsNum(); i++)
    {
        tmp_dis = m_obstacles[i]->NearestDistance(t_p);
        if (tmp_dis < dis)
            dis = tmp_dis;
        if (dis <= 0)
            return dis;
    }

    return dis;
}

bool ObstacleMap::collisionDetect(const Common::Vec2 p1, const Common::Vec2 p2)
{
    float coss, sinn;
    coss = (p2.x - p1.x) / p1.distanceTo(p2);
    sinn = (p2.y - p1.y) / p1.distanceTo(p2);

    Common::Vec2 current = p1;

    while (current.distanceTo(p2) > 10.0f)
    {
        if (isInObstacle(current))
            return true;

        current.x += coss * 10.0f;
        current.y += sinn * 10.0f;
    }

    return false;
}

void ObstacleMap::resetMap()
{
    m_obstacles.clear();
}

int ObstacleMap::getObsNum()
{
    return m_obstacles.size();
}
} // namespace Tyr::Soccer
