#include "trajectory.h"

namespace Tyr::Soccer
{
Trajectory Trajectory::MakeDssTrajectory(const Common::RobotState &state, const Common::Vec2 &a_acc, const float a_dec,
                                      const float a_dt)
{
    Trajectory trajectory;

    const Common::Vec2 p0 = state.position;
    const Common::Vec2 v0 = state.velocity;
    const Common::Vec2 a0 = a_acc;

    const float t1 = a_dt;

    const TrajectoryPiece piece_acc {a0, v0, p0, 0.f, t1};
    trajectory.addPiece(piece_acc);

    const Common::Vec2 p1 = piece_acc.getPosition(t1);
    const Common::Vec2 v1 = piece_acc.getVelocity(t1);
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

Trajectory Trajectory::MakeOpponentDssTrajectory(const Common::RobotState &state, const float a_dec)
{
    Trajectory trajectory;

    const Common::Vec2 p1 = state.position;
    const Common::Vec2 v1 = state.velocity;
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
