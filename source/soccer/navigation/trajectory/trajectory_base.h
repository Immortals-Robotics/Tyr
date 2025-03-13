#pragma once

namespace Tyr::Soccer
{
struct VelocityProfile;

template<typename T>
class TrajectoryBase
{
public:
    TrajectoryBase() = default;
    virtual ~TrajectoryBase() = default;

    virtual T getPosition(const float t) const = 0;
    virtual T getVelocity(const float t) const = 0;
    virtual T getAcceleration(const float t) const = 0;

    virtual float getStartTime() const = 0;
    virtual float getEndTime() const = 0;

    virtual bool hasCollision(const TrajectoryBase &other, float r) const = 0;
    virtual bool hasCollision(const ObstacleMap &map) const = 0;

    virtual void draw() const
    {
    }

    float getDuration() const
    {
        return getEndTime() - getStartTime();
    }
};
} // namespace Tyr::Soccer
