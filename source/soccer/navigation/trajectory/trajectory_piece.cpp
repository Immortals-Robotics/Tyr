#include "trajectory_piece.h"

#include "../obstacle/map.h"

namespace Tyr::Soccer
{
bool TrajectoryPiece::hasCollision(const TrajectoryPiece &other, const float r) const
{
    const float t_start = std::max(m_t_start, other.m_t_start);
    const float t_end   = std::min(m_t_end, other.m_t_end);

    if (t_end <= t_start)
    {
        return false;
    }

    // TODO: this is horrible :/
    return hasCollision(other, r, t_start) || hasCollision(other, r, t_end) ||
           hasCollision(other, r, (t_start + t_end) / 2.f);
}

TrajectoryPiece TrajectoryPiece::makeOppPiece(const Common::RobotState &state)
{
    return {{}, state.velocity, state.position, 0.f, std::numeric_limits<float>::max()};
}

bool TrajectoryPiece::hasCollision(const ObstacleMap &map) const
{
    if (m_t_end <= m_t_start)
    {
        return false;
    }

    // TODO: this is horrible :/
    return
        map.isInObstacle(getPosition(m_t_start)) ||
        map.isInObstacle(getPosition(m_t_end)) ||
        map.isInObstacle(getPosition((m_t_start + m_t_end) / 2.f));
}

bool TrajectoryPiece::hasCollision(const TrajectoryPiece &other, const float r, const float t) const
{
    const float t_start = std::max(m_t_start, other.m_t_start);
    const float t_end = std::min(m_t_end, other.m_t_end);

    if (t_end <= t_start || t < t_start || t > t_end)
    {
        return false;
    }

    return distanceTo(other, t) <= r;
}
} // namespace Tyr::Soccer
