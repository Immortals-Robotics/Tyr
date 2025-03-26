#pragma once

#include "trajectory.h"
#include "trajectory_constant_acc.h"

namespace Tyr::Soccer
{
struct VelocityProfile;

template<typename T, TrajectoryConcept<T> Piece>
class TrajectoryPieced
{
protected:
    static constexpr size_t kMaxParts = 3;
    std::array<Piece, kMaxParts> m_pieces;
    size_t m_pieces_count = 0;

    static constexpr size_t kInvalidIdx = std::numeric_limits<size_t>::max();

    size_t findPieceIdx(const float t) const
    {
        for (size_t idx = 0; idx < m_pieces_count; idx++)
        {
            if (t <= m_pieces[idx].t_end)
            {
                return idx;
            }
        }

        Common::logError("no piece found for t = {}", t);
        return kInvalidIdx;
    }

    const Piece& firstPiece() const
    {
        assert(m_pieces_count > 0);
        return m_pieces[0];
    }

    const Piece& lastPiece() const
    {
        assert(m_pieces_count > 0);
        return m_pieces[m_pieces_count - 1];
    }

public:
    TrajectoryPieced() = default;

    void addPiece(const Piece &piece)
    {
        assert(m_pieces_count < kMaxParts);
        m_pieces[m_pieces_count] = piece;
        m_pieces_count++;
    }

    T getPosition(float t) const
    {
        t = std::clamp(t, getStartTime(), getEndTime());
        const int idx = findPieceIdx(t);
        return idx == kInvalidIdx ? T() : m_pieces[idx].getPosition(t);
    }

    T getVelocity(float t) const
    {
        t = std::clamp(t, getStartTime(), getEndTime());
        const int idx = findPieceIdx(t);
        return idx == kInvalidIdx ? T() : m_pieces[idx].getVelocity(t);
    }

    T getAcceleration(float t) const
    {
        t = std::clamp(t, getStartTime(), getEndTime());
        const int idx = findPieceIdx(t);
        return idx == kInvalidIdx ? T() : m_pieces[idx].acc;
    }

    float getStartTime() const
    {
        return m_pieces_count == 0 ? 0.0f : firstPiece().getStartTime();
    }

    float getEndTime() const
    {
        return m_pieces_count == 0 ? 0.0f : lastPiece().getEndTime();
    }

    float getDuration() const
    {
        return std::max(0.0f, getEndTime() - getStartTime());
    }
};
} // namespace Tyr::Soccer
