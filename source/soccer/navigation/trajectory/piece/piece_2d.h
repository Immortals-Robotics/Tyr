#pragma once

#include "piece_base.h"

namespace Tyr::Soccer
{
class ObstacleMap;

class TrajectoryPiece2D : public TrajectoryPieceBase<Common::Vec2>
{
private:
    bool hasCollision(const TrajectoryPiece2D &other, float r, float t) const;

    inline float distanceTo(const TrajectoryPiece2D &other, const float t) const
    {
        return getPosition(t).distanceTo(other.getPosition(t));
    }

public:
    TrajectoryPiece2D() = default;

    static TrajectoryPiece2D makeOppPiece(const Common::RobotState& state);

    bool hasCollision(const ObstacleMap &map) const;
    bool hasCollision(const TrajectoryPiece2D &other, float r) const;
};
} // namespace Tyr::Soccer
