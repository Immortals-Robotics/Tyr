#include "trajectory.h"

namespace Tyr::Soccer
{
Trajectory Trajectory::makeDssTrajectory(const TrajectoryPiece& cmd_piece, const float a_dec)
{
    Trajectory trajectory;

    trajectory.addPiece(cmd_piece);

    const float t1 = cmd_piece.getEndTime();
    const Common::Vec2 p1 = cmd_piece.getPosition(t1);
    const Common::Vec2 v1 = cmd_piece.getVelocity(t1);
    const Common::Vec2 a1 = v1.normalized() * (-a_dec);

    const float dec_dt = v1.length() / a_dec;
    const float t2     = t1 + dec_dt;

    const TrajectoryPiece piece_dec {a1, v1, p1, t1, t2};
    trajectory.addPiece(piece_dec);

    const Common::Vec2 p2 = piece_dec.getPosition(t2);

    const TrajectoryPiece piece_stopped{{}, {}, p2, t2, std::numeric_limits<float>::max()};
    trajectory.addPiece(piece_stopped);

    return trajectory;
}

Trajectory Trajectory::makeStopDssTrajectory(const TrajectoryPiece& cmd_piece, const float a_dec)
{
    Trajectory trajectory;

    const float t1 = cmd_piece.getStartTime();
    const Common::Vec2 p1 = cmd_piece.getPosition(t1);
    const Common::Vec2 v1 = cmd_piece.getVelocity(t1);
    const Common::Vec2 a1 = v1.normalized() * (-a_dec);

    const float dec_dt = v1.length() / a_dec;
    const float t2     = dec_dt;

    const TrajectoryPiece piece_dec{a1, v1, p1, 0.f, t2};
    trajectory.addPiece(piece_dec);

    const Common::Vec2 p2 = piece_dec.getPosition(t2);

    const TrajectoryPiece piece_stopped{{}, {}, p2, t2, std::numeric_limits<float>::max()};
    trajectory.addPiece(piece_stopped);

    return trajectory;
}

bool Trajectory::hasCollision(const Trajectory &other, const float r) const
{
    for (const TrajectoryPiece &piece : m_pieces)
    {
        for (const TrajectoryPiece &otherPiece : other.m_pieces)
        {
            if (piece.hasCollision(otherPiece, r))
            {
                return true;
            }
        }
    }

    return false;
}

bool Trajectory::hasCollision(const ObstacleMap &map) const
{
    for (const TrajectoryPiece &piece : m_pieces)
    {
        if (piece.hasCollision(map))
        {
            return true;
        }
    }

    return false;
}
} // namespace Tyr::Soccer
