#pragma once

#include "trajectory.h"

namespace Tyr::Soccer
{
template<typename SubTrajectory>
class TrajectoryChained final : public Trajectory<Common::Vec2>
{
protected:
    SubTrajectory m_trajectory[2];
    float m_cut_time = 0.0f;

public:
    TrajectoryChained() = default;

    TrajectoryChained(const SubTrajectory& first, const SubTrajectory& second, float cut_time)
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

    void draw(const Common::Color color) const override
    {
        for (float t = getStartTime(); t < getEndTime(); t += 0.1f)
        {
            Common::debug().draw(getPosition(t), color);
        }
    }

    const SubTrajectory& getFirstTrajectory() const
    {
        return m_trajectory[0];
    }

    const SubTrajectory& getSecondTrajectory() const
    {
        return m_trajectory[1];
    }

    float getCutTime() const
    {
        return m_cut_time;
    }
};
} // namespace Tyr::Soccer
