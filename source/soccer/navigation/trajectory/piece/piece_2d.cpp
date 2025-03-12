#include "piece_2d.h"

#include "../../obstacle/map.h"

namespace Tyr::Soccer
{
bool TrajectoryPiece2D::hasCollision(const TrajectoryPiece2D &other, const float r) const
{
    const float t_start = std::max(this->t_start, other.t_start);
    const float t_end   = std::min(this->t_end, other.t_end);

    if (t_end <= t_start)
    {
        return false;
    }

    // TODO: this is horrible :/
    return hasCollision(other, r, t_start) || hasCollision(other, r, t_end) ||
           hasCollision(other, r, (t_start + t_end) / 2.f);
}

TrajectoryPiece2D TrajectoryPiece2D::makeOppPiece(const Common::RobotState &state)
{
    TrajectoryPiece2D piece{};
    piece.v_start = state.velocity;
    piece.p_start = state.position;
    piece.t_end = std::numeric_limits<float>::max();

    return piece;
}

bool TrajectoryPiece2D::hasCollision(const ObstacleMap &map) const
{
    if (t_end <= t_start)
    {
        return false;
    }

    // TODO: this is horrible :/
    return
        map.isInObstacle(getPosition(t_start)) ||
        map.isInObstacle(getPosition(t_end)) ||
        map.isInObstacle(getPosition((t_start + t_end) / 2.f));
}

bool TrajectoryPiece2D::hasCollision(const TrajectoryPiece2D &other, const float r, const float t) const
{
    const float t_start = std::max(this->t_start, other.t_start);
    const float t_end = std::min(this->t_end, other.t_end);

    if (t_end <= t_start || t < t_start || t > t_end)
    {
        return false;
    }

    return distanceTo(other, t) <= r;
}
} // namespace Tyr::Soccer
