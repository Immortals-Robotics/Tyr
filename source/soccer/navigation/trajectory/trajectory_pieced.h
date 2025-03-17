#pragma once

#include "piece/piece_base.h"
#include "trajectory_base.h"

namespace Tyr::Soccer
{
struct VelocityProfile;

template<typename T, typename Piece = TrajectoryPieceBase<T>>
class TrajectoryPieced : public TrajectoryBase<T>
{
protected:
    static constexpr size_t kMaxParts = 4;
    std::vector<Piece> m_pieces;

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
        return -1;
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
        return idx == -1 ? T() : m_pieces[idx].getPosition(t);
    }

    T getVelocity(const float t) const override
    {
        const int idx = findPieceIdx(t);
        return idx == -1 ? T() : m_pieces[idx].getVelocity(t);
    }

    T getAcceleration(const float t) const override
    {
        const int idx = findPieceIdx(t);
        return idx == -1 ? T() : m_pieces[idx].acc;
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
