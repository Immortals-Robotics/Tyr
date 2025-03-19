#pragma once

#include "trajectory.h"
#include "trajectory_constant_acc.h"

namespace Tyr::Soccer
{
struct VelocityProfile;

template<typename T, typename Piece = TrajectoryConstantAcc<T>>
class TrajectoryPieced : public Trajectory<T>
{
protected:
    static constexpr size_t kMaxParts = 4;
    std::vector<Piece> m_pieces;

    static constexpr size_t kInvalidIdx = std::numeric_limits<size_t>::max();

    inline size_t findPieceIdx(const float t) const
    {
        for (size_t idx = 0; idx < m_pieces.size(); idx++)
        {
            if (t <= m_pieces[idx].t_end)
            {
                return idx;
            }
        }

        Common::logError("no piece found for t = {}", t);
        return kInvalidIdx;
    }

public:
    TrajectoryPieced()
    {
        m_pieces.reserve(kMaxParts);
    }

    void addPiece(const Piece &piece)
    {
        m_pieces.push_back(piece);
    }

    T getPosition(const float t) const override
    {
        const int idx = findPieceIdx(t);
        return idx == kInvalidIdx ? T() : m_pieces[idx].getPosition(t);
    }

    T getVelocity(const float t) const override
    {
        const int idx = findPieceIdx(t);
        return idx == kInvalidIdx ? T() : m_pieces[idx].getVelocity(t);
    }

    T getAcceleration(const float t) const override
    {
        const int idx = findPieceIdx(t);
        return idx == kInvalidIdx ? T() : m_pieces[idx].acc;
    }

    float getStartTime() const override
    {
        return m_pieces.empty() ? 0.f : m_pieces.front().t_start;
    }

    float getEndTime() const override
    {
        return m_pieces.empty() ? 0.f : m_pieces.back().t_end;
    }
};
} // namespace Tyr::Soccer
