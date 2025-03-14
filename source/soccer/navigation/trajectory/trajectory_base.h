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

    virtual T getPosition(float t) const = 0;
    virtual T getVelocity(float t) const = 0;
    virtual T getAcceleration(float t) const = 0;

    virtual float getStartTime() const = 0;
    virtual float getEndTime() const = 0;

    virtual bool hasCollision(const TrajectoryBase &other, float r, float look_ahead = 3.0f, float step_t = 0.1f) const = 0;
    virtual bool hasCollision(const ObstacleMap &map, float look_ahead = 3.0f, float step_t = 0.1f) const = 0;

    virtual void draw() const
    {
    }

    float getDuration() const
    {
        return getEndTime() - getStartTime();
    }
};
} // namespace Tyr::Soccer
