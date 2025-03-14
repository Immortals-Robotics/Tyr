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

    bool hasCollision(const TrajectoryBase &other, float r, float step_t = 0.1f, float look_ahead = 3.0f) const override
    {
        (void) other;
        (void) r;
        (void) step_t;
        (void) look_ahead;
        return false;
    }

    bool hasCollision(const ObstacleMap &map, float step_t = 0.1f, float look_ahead = 3.0f) const override
    {
        (void) map;
        (void) step_t;
        (void) look_ahead;
        return false;
    }
};
} // namespace Tyr::Soccer
