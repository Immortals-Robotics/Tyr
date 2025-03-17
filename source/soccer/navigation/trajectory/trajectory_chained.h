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

    std::pair<bool, float> hasCollision(const TrajectoryBase &other, float r, float look_ahead = 3.0f,
                                        float step_t = 0.1f) const override
    {
        const float look_ahead_0 = std::min(m_cut_time, look_ahead);
        const float look_ahead_1 = std::max(0.0f, look_ahead - m_cut_time);

        const auto collision_0 = m_trajectory[0].hasCollision(other, r, look_ahead_0, step_t);
        const auto collision_1 = m_trajectory[1].hasCollision(other, r, look_ahead_1, step_t);

        return {
            collision_0.first || collision_1.first,
            std::min(collision_0.second, collision_1.second)};
    }

    std::pair<bool, float> hasCollision(const ObstacleMap &map, float look_ahead = 3.0f,
                                        float step_t = 0.1f) const override
    {
        const float look_ahead_0 = std::min(m_cut_time, look_ahead);
        const float look_ahead_1 = std::max(0.0f, look_ahead - m_cut_time);

        const auto collision_0 = m_trajectory[0].hasCollision(map, look_ahead_0, step_t);
        const auto collision_1 = m_trajectory[1].hasCollision(map, look_ahead_1, step_t);

        return {
            collision_0.first || collision_1.first,
            std::min(collision_0.second, collision_1.second)};
    }

    std::pair<bool, float> reachFree(const ObstacleMap &map, const float look_ahead = 3.0f, const float step_t = 0.1f) const override
    {
        const float look_ahead_0 = std::min(m_cut_time, look_ahead);
        const float look_ahead_1 = std::max(0.0f, look_ahead - m_cut_time);

        const auto free_0 = m_trajectory[0].reachFree(map, look_ahead_0, step_t);
        const auto free_1 = m_trajectory[1].reachFree(map, look_ahead_1, step_t);

        return {
            free_0.first || free_1.first,
            std::min(free_0.second, free_1.second)};
    }

    void draw(const Common::Color color) const override
    {
        for (float t = getStartTime(); t < getEndTime(); t += 0.1f)
        {
            Common::debug().draw(getPosition(t), color);
        }
    }

    const Trajectory& getFirstTrajectory() const
    {
        return m_trajectory[0];
    }

    const Trajectory& getSecondTrajectory() const
    {
        return m_trajectory[1];
    }

    float getCutTime() const
    {
        return m_cut_time;
    }
};
} // namespace Tyr::Soccer
