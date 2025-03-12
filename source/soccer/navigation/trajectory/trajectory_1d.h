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

    bool hasCollision(const TrajectoryBase &other, float r) const override
    {
        (void) other;
        (void) r;
        return false;
    }

    bool hasCollision(const ObstacleMap &map) const override
    {
        (void) map;
        return false;
    }
};
} // namespace Tyr::Soccer
