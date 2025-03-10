#pragma once

#include "trajectory_piece.h"

namespace Tyr::Soccer
{
class Trajectory
{
private:
    static constexpr size_t kMaxParts = 4;
    std::vector<TrajectoryPiece> m_pieces;

    inline size_t findPieceIdx(const float t) const
    {
        for (size_t idx = 0; idx < kMaxParts; idx++)
        {
            if (t <= m_pieces[idx].getEndTime())
            {
                return idx;
            }
        }

        Common::logError("no piece found for t = {}", t);
        return m_pieces.size() - 1;
    }

public:
    Trajectory()
    {
        m_pieces.reserve(kMaxParts);
    }

    // makes a trajectory that consists of:
    // 1- accelerating with a_acc for a_dt
    // 2- decelerating with a_dec until full stop
    // 3- being at full stop till the end of time (literally)
    static Trajectory MakeDssTrajectory(const Common::RobotState &state, const Common::Vec2 &a_acc, float a_dec, float a_dt);

    // makes a trajectory that consists of:
    // 1- decelerating with a_dec until full stop
    // 2- being at full stop till the end of time (literally)
    static Trajectory MakeOpponentDssTrajectory(const Common::RobotState &state, float a_dec);

    void addPiece(const TrajectoryPiece &piece)
    {
        m_pieces.push_back(piece);
    }

    Common::Vec2 getPosition(const float t) const
    {
        // TODO: handle out of range t
        return m_pieces[findPieceIdx(t)].getPosition(t);
    }

    Common::Vec2 getVelocity(const float t) const
    {
        // TODO: handle out of range t
        return m_pieces[findPieceIdx(t)].getVelocity(t);
    }

    Common::Vec2 getAcceleration(const float t) const
    {
        // TODO: handle out of range t
        return m_pieces[findPieceIdx(t)].getAcceleration();
    }

    float getStartTime() const
    {
        return m_pieces.front().getStartTime();
    }

    float getEndTime() const
    {
        return m_pieces.back().getEndTime();
    }

    float getDuration() const
    {
        return getEndTime() - getStartTime();
    }

    bool hasCollision(const Trajectory &other, float r) const;
    bool hasCollision(const ObstacleMap &map) const;
};
} // namespace Tyr::Soccer
