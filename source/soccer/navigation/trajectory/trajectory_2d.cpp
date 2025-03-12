#include "trajectory_2d.h"

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

bool Trajectory2D::hasCollision(const TrajectoryBase &other, const float r) const
{
    const Trajectory2D& other_2d = static_cast<const Trajectory2D&>(other);

    for (const TrajectoryPiece2D &piece : m_pieces)
    {
        for (const TrajectoryPiece2D &otherPiece : other_2d.m_pieces)
        {
            if (piece.hasCollision(otherPiece, r))
            {
                return true;
            }
        }
    }

    return false;
}

bool Trajectory2D::hasCollision(const ObstacleMap &map) const
{
    for (const TrajectoryPiece2D &piece : m_pieces)
    {
        if (piece.hasCollision(map))
        {
            return true;
        }
    }

    return false;
}
} // namespace Tyr::Soccer
