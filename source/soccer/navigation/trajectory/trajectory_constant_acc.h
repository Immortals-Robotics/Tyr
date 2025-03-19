#pragma once

namespace Tyr::Soccer
{
class ObstacleMap;

// defines a constant-acceleration piece of trajectory
template<typename T>
class TrajectoryConstantAcc : public Trajectory<T>
{
public:
    T acc;

    T v_start;
    T p_start;

    float t_start = 0.f;
    float t_end = 0.f;

    TrajectoryConstantAcc() = default;

    T getPosition(const float t) const override
    {
        const float dt = t - t_start;
        return p_start + v_start * dt + acc * (dt * dt * .5f);
    }

    T getVelocity(const float t) const override
    {
        const float dt = t - t_start;
        return v_start + acc * dt;
    }

    T getAcceleration(const float t) const override
    {
        return acc;
    }

    float getStartTime() const override
    {
        return t_start;
    }

    float getEndTime() const override
    {
        return t_end;
    }
};

using TrajectoryPiece2D = TrajectoryConstantAcc<Common::Vec2>;
using TrajectoryPiece1D = TrajectoryConstantAcc<float>;
} // namespace Tyr::Soccer
