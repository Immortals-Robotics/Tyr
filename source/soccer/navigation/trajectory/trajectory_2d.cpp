#include "trajectory_2d.h"

#include "../obstacle/map.h"

namespace Tyr::Soccer
{
Trajectory2D Trajectory2D::makeDssTrajectory(const TrajectoryPiece2D& cmd_piece, const float a_dec)
{
    Trajectory2D trajectory;

    trajectory.addPiece(cmd_piece);

    const float t1 = cmd_piece.t_end;
    const Common::Vec2 p1 = cmd_piece.getPosition(t1);
    const Common::Vec2 v1 = cmd_piece.getVelocity(t1);
    const Common::Vec2 a1 = v1.normalized() * (-a_dec);

    const float dec_dt = v1.length() / a_dec;
    const float t2     = t1 + dec_dt;

    TrajectoryPiece2D piece_dec{};
    piece_dec.acc = a1;
    piece_dec.v_start = v1;
    piece_dec.p_start = p1;
    piece_dec.t_start = t1;
    piece_dec.t_end = t2;
    trajectory.addPiece(piece_dec);

    const Common::Vec2 p2 = piece_dec.getPosition(t2);

    TrajectoryPiece2D piece_stopped{};
    piece_stopped.p_start = p2;
    piece_stopped.t_start = t2;
    piece_stopped.t_end = std::numeric_limits<float>::max();
    trajectory.addPiece(piece_stopped);

    return trajectory;
}

Trajectory2D Trajectory2D::makeStopDssTrajectory(const TrajectoryPiece2D& cmd_piece, const float a_dec)
{
    Trajectory2D trajectory;

    const float t1 = cmd_piece.t_start;
    const Common::Vec2 p1 = cmd_piece.getPosition(t1);
    const Common::Vec2 v1 = cmd_piece.getVelocity(t1);
    const Common::Vec2 a1 = v1.normalized() * (-a_dec);

    const float dec_dt = v1.length() / a_dec;
    const float t2     = dec_dt;

    TrajectoryPiece2D piece_dec{};
    piece_dec.acc = a1;
    piece_dec.v_start = v1;
    piece_dec.p_start = p1;
    piece_dec.t_end = t2;
    trajectory.addPiece(piece_dec);

    const Common::Vec2 p2 = piece_dec.getPosition(t2);

    TrajectoryPiece2D piece_stopped{};
    piece_stopped.p_start = p2;
    piece_stopped.t_start = t2;
    piece_stopped.t_end = std::numeric_limits<float>::max();
    trajectory.addPiece(piece_stopped);

    return trajectory;
}

// TODO: this duplicates the one in trajectory_2d_xy
std::pair<bool, float> Trajectory2D::hasCollision(const TrajectoryBase &other, const float r, const float look_ahead,
                                                  const float step_t) const
{
    const float t_start = std::max(this->getStartTime(), other.getStartTime());
    const float t_end_raw = std::min(this->getEndTime(), other.getEndTime());
    const float t_end = std::min(t_end_raw, t_start + look_ahead);

    for (float t = t_start; t < t_end; t += step_t)
    {
        const Common::Vec2 pos = getPosition(t);
        const Common::Vec2 other_pos = other.getPosition(t);

        if (pos.distanceTo(other_pos) <= r)
        {
            return {true, t};
        }
    }

    return {false, std::numeric_limits<float>::max()};
}

// TODO: this duplicates the one in trajectory_2d_xy
std::pair<bool, float> Trajectory2D::hasCollision(const ObstacleMap &map, const float look_ahead,
                                                  const float step_t) const
{
    const float t_end = std::min(getEndTime(), getStartTime() + look_ahead);

    for (float t = getStartTime(); t < t_end; t += step_t)
    {
        const Common::Vec2 pos = getPosition(t);

        if (map.inside(pos))
        {
            return {true, t};
        }
    }

    return {false, std::numeric_limits<float>::max()};
}

std::pair<bool, float> Trajectory2D::reachFree(const ObstacleMap &map, float look_ahead, float step_t) const
{
    const float t_end = std::min(getEndTime(), getStartTime() + look_ahead);

    for (float t = getStartTime(); t < t_end; t += step_t)
    {
        const Common::Vec2 pos = getPosition(t);

        if (!map.inside(pos))
        {
            return {true, t};
        }
    }

    return {false, std::numeric_limits<float>::max()};
}
} // namespace Tyr::Soccer
