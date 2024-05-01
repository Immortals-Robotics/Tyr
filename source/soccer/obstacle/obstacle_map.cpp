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

        m_obstacles.emplace_back(std::make_unique<CircleObstacle>(Common::Circle{Common::Vec2{_x, _y}, _r}));
    }
}
void ObsMap::AddRectangle(float _x, float _y, float _w, float _h)
{
    Common::Vec2 start{};
    Common::Vec2 size{};

    if (_w < 0)
    {
        start.x = _x + _w;
        size.x  = -_w;
    }
    else
    {
        start.x = _x;
        size.x  = _w;
    }

    if (_h < 0)
    {
        start.y = _y + _h;
        // TODO: shouldn't this be -h?
        size.y = _h;
    }
    else
    {
        start.y = _y;
        size.y  = _h;
    }

    m_obstacles.emplace_back(std::make_unique<RectangleObstacle>(Common::Rect{start, start + size}));
}

bool ObsMap::IsInObstacle(const Common::Vec2 t_p)
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

float ObsMap::NearestDistance(const Common::Vec2 t_p)
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
