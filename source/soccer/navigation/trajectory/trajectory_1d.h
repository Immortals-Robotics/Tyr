#pragma once

#include "piece/piece_1d.h"
#include "trajectory_pieced.h"

namespace Tyr::Soccer
{
struct VelocityProfile;

class Trajectory1D final : public TrajectoryPieced<float, TrajectoryPiece1D>
{
public:
    Trajectory1D() = default;

    std::pair<bool, float> hasCollision(const TrajectoryBase &other, float r, float look_ahead = 3.0f,
                                        float step_t = 0.1f) const override
    {
        (void) other;
        (void) r;
        (void) look_ahead;
        (void) step_t;
        return {};
    }

    std::pair<bool, float> hasCollision(const ObstacleMap &map, float look_ahead = 3.0f,
                                        float step_t = 0.1f) const override
    {
        (void) map;
        (void) look_ahead;
        (void) step_t;
        return {};
    }

    std::pair<bool, float> reachFree(const ObstacleMap &map, float look_ahead, float step_t) const override
    {
        (void) map;
        (void) look_ahead;
        (void) step_t;
        return {true, 0.f};
    }
};
} // namespace Tyr::Soccer
