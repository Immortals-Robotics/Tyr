#include "piece_2d.h"

namespace Tyr::Soccer
{
TrajectoryPiece2D TrajectoryPiece2D::makeOppPiece(const Common::RobotState &state)
{
    TrajectoryPiece2D piece{};
    piece.v_start = state.velocity;
    piece.p_start = state.position;
    piece.t_end = std::numeric_limits<float>::max();

    return piece;
}
} // namespace Tyr::Soccer
