#pragma once

namespace Tyr::Soccer
{
class ObstacleMap;

// defines a constant-acceleration piece of trajectory
class TrajectoryPiece
{
private:
    bool hasCollision(const TrajectoryPiece &other, float r, float t) const;

    inline float distanceTo(const TrajectoryPiece &other, const float t) const
    {
        return getPosition(t).distanceTo(other.getPosition(t));
    }

    Common::Vec2 m_acc;

    Common::Vec2 m_v_start;
    Common::Vec2 m_p_start;

    float m_t_start = 0.f;
    float m_t_end = 0.f;

public:
    TrajectoryPiece() = default;

    TrajectoryPiece(const Common::Vec2 acc, const Common::Vec2 v_start, const Common::Vec2 p_start,
                    const float t_start,const float t_end)
        : m_acc(acc), m_v_start(v_start), m_p_start(p_start), m_t_start(t_start), m_t_end(t_end)
    {}

    inline Common::Vec2 getPosition(const float t) const
    {
        const float dt = t - m_t_start;
        return m_p_start + m_v_start * dt + m_acc * (dt * dt * .5f);
    }

    inline Common::Vec2 getVelocity(const float t) const
    {
        const float dt = t - m_t_start;
        return m_v_start + m_acc * dt;
    }

    inline Common::Vec2 getAcceleration() const
    {
        return m_acc;
    }

    inline float getStartTime() const
    {
        return m_t_start;
    }

    inline float getEndTime() const
    {
        return m_t_end;
    }

    inline float getDuration() const
    {
        return m_t_end - m_t_start;
    }

    bool hasCollision(const ObstacleMap &map) const;
    bool hasCollision(const TrajectoryPiece &other, float r) const;
};
} // namespace Tyr::Soccer
