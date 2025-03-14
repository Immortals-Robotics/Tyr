#pragma once

#include "trajectory_2d_xy.h"

namespace Tyr::Soccer
{
template<typename Trajectory>
class TrajectoryChained final : public TrajectoryBase<Common::Vec2>
{
protected:
    Trajectory m_trajectory[2];
    float m_cut_time = 0.0f;

public:
    TrajectoryChained() = default;

    TrajectoryChained(const Trajectory& first, const Trajectory& second, float cut_time)
    {
        m_trajectory[0] = first;
        m_trajectory[1] = second;
        m_cut_time = cut_time;
    }

    Common::Vec2 getPosition(const float t) const override
    {
        return t < m_cut_time
            ? m_trajectory[0].getPosition(t)
            : m_trajectory[1].getPosition(t - m_cut_time);
    }

    Common::Vec2 getVelocity(const float t) const override
    {
        return t < m_cut_time
            ? m_trajectory[0].getVelocity(t)
            : m_trajectory[1].getVelocity(t - m_cut_time);
    }

    Common::Vec2 getAcceleration(const float t) const override
    {
        return t < m_cut_time
            ? m_trajectory[0].getAcceleration(t)
            : m_trajectory[1].getAcceleration(t - m_cut_time);
    }

    float getStartTime() const override
    {
        return m_trajectory[0].getStartTime();
    }

    float getEndTime() const override
    {
        return m_trajectory[1].getEndTime() + m_cut_time;
    }

    bool hasCollision(const TrajectoryBase &other, float r, float look_ahead = 3.0f, float step_t = 0.1f) const override
    {
        const float look_aheads[2] = {
            std::min(m_cut_time, look_ahead),
            std::max(0.0f, look_ahead - m_cut_time)};

        return
            m_trajectory[0].hasCollision(other, r, look_aheads[0], step_t) ||
            m_trajectory[1].hasCollision(other, r, look_aheads[1], step_t);
    }

    bool hasCollision(const ObstacleMap &map, float look_ahead = 3.0f, float step_t = 0.1f) const override
    {
        const float look_aheads[2] = {
            std::min(m_cut_time, look_ahead),
            std::max(0.0f, look_ahead - m_cut_time)};

        return
            m_trajectory[0].hasCollision(map, look_aheads[0], step_t) ||
            m_trajectory[1].hasCollision(map, look_aheads[1], step_t);
    }

    void draw() const override
    {
        for (float t = getStartTime(); t < getEndTime(); t += 0.1f)
        {
            Common::debug().draw(getPosition(t), Common::Color::magenta());
        }
    }
};
} // namespace Tyr::Soccer
