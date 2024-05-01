#include "obstacle_new.h"

namespace Tyr::Soccer
{
ObsMap::ObsMap()
{}

void ObsMap::addCircle(const Common::Circle t_circle)
{
    // TODO: verify if this check is needed
    if (t_circle.r > 0)
    {
        m_obstacles.emplace_back(std::make_unique<CircleObstacle>(t_circle));
    }
}
void ObsMap::addRectangle(const Common::Rect t_rect)
{
    m_obstacles.emplace_back(std::make_unique<RectangleObstacle>(t_rect));
}

bool ObsMap::isInObstacle(const Common::Vec2 t_p)
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

float ObsMap::nearestDistance(const Common::Vec2 t_p)
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

void ObsMap::resetMap()
{
    m_obstacles.clear();
}

int ObsMap::getObsNum()
{
    return m_obstacles.size();
}
} // namespace Tyr::Soccer
