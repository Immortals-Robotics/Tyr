#pragma once

#include "piece_base.h"

namespace Tyr::Soccer
{
class ObstacleMap;

class TrajectoryPiece2D : public TrajectoryPieceBase<Common::Vec2>
{
public:
    TrajectoryPiece2D() = default;
    static TrajectoryPiece2D makeOppPiece(const Common::RobotState& state);
};
} // namespace Tyr::Soccer
