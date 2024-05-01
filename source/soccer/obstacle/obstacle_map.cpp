#include "obstacle_new.h"

namespace Tyr::Soccer
{
ObsMap::ObsMap()
{}

void ObsMap::AddCircle(float _x, float _y, float _r)
{
    // TODO: verify if this check is needed
    if (_r > 0)
    {
        m_obstacles.emplace_back(std::make_unique<CircleObstacle>(_x, _y, _r));
    }
}
void ObsMap::AddRectangle(float _x, float _y, float _w, float _h)
{
    m_obstacles.emplace_back(std::make_unique<RectangleObstacle>(_x, _y, _w, _h));
}

bool ObsMap::IsInObstacle(float _x, float _y)
{
    for (int i = 0; i < getObsNum(); i++)
    {
        if (m_obstacles[i]->IsInObstacle(_x, _y))
            return true;
    }
    if (std::fabs(_x) > 10000 ||
        std::fabs(_y) > 10000) // If the  values where getting to much (this fixes the errors for now...
    {
        return true;
    }
    return false;
}

float ObsMap::NearestDistance(float _x, float _y)
{
    float dis = std::numeric_limits<float>::max();
    float tmp_dis;
    for (int i = 0; i < getObsNum(); i++)
    {
        tmp_dis = m_obstacles[i]->NearestDistance(_x, _y);
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
