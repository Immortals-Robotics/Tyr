#pragma once

namespace Tyr::Soccer
{
class BaseObstacle
{
public:
    virtual ~BaseObstacle() = default;

    virtual bool  IsInObstacle(Common::Vec2 t_point)    = 0;
    virtual float NearestDistance(Common::Vec2 t_point) = 0;
};
} // namespace Tyr::Soccer
