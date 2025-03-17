#pragma once

namespace Tyr::Soccer
{
class ObstacleMap;

// defines a constant-acceleration piece of trajectory
template<typename T>
class TrajectoryPieceBase
{
public:
    T acc;

    T v_start;
    T p_start;

    float t_start = 0.f;
    float t_end = 0.f;

    TrajectoryPieceBase() = default;

    inline T getPosition(const float t) const
    {
        const float dt = t - t_start;
        return p_start + v_start * dt + acc * (dt * dt * .5f);
    }

    inline T getVelocity(const float t) const
    {
        const float dt = t - t_start;
        return v_start + acc * dt;
    }

    inline float getDuration() const
    {
        return t_end - t_start;
    }
};
} // namespace Tyr::Soccer
