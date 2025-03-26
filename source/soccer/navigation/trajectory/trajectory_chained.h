#pragma once

#include "trajectory.h"

namespace Tyr::Soccer
{
template<typename T, TrajectoryConcept<T> SubTrajectory>
class TrajectoryChained
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

    T getPosition(const float t) const
    {
        return t < m_cut_time
            ? m_trajectory[0].getPosition(t)
            : m_trajectory[1].getPosition(t - m_cut_time);
    }

    T getVelocity(const float t) const
    {
        return t < m_cut_time
            ? m_trajectory[0].getVelocity(t)
            : m_trajectory[1].getVelocity(t - m_cut_time);
    }

    T getAcceleration(const float t) const
    {
        return t < m_cut_time
            ? m_trajectory[0].getAcceleration(t)
            : m_trajectory[1].getAcceleration(t - m_cut_time);
    }

    float getStartTime() const
    {
        return m_trajectory[0].getStartTime();
    }

    float getEndTime() const
    {
        return m_trajectory[1].getEndTime() + m_cut_time;
    }

    float getDuration() const
    {
        return std::max(0.0f, getEndTime() - getStartTime());
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
