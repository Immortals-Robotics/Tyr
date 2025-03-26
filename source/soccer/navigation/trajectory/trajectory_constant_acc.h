#pragma once

namespace Tyr::Soccer
{
class ObstacleMap;

// defines a constant-acceleration piece of trajectory
template<typename T>
class TrajectoryConstantAcc
{
public:
    T acc;

    T v_start;
    T p_start;

    float t_start = 0.f;
    float t_end = 0.f;

    TrajectoryConstantAcc() = default;

    T getPosition(const float t) const
    {
        const float dt = t - t_start;
        return p_start + v_start * dt + acc * (dt * dt * .5f);
    }

    T getVelocity(const float t) const
    {
        const float dt = t - t_start;
        return v_start + acc * dt;
    }

    T getAcceleration(const float t) const
    {
        (void) t;
        return acc;
    }

    float getStartTime() const
    {
        return t_start;
    }

    float getEndTime() const
    {
        return t_end;
    }

    float getDuration() const
    {
        return std::max(0.0f, getEndTime() - getStartTime());
    }
};
} // namespace Tyr::Soccer
