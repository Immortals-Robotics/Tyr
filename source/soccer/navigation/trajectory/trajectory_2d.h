#pragma once

#include "trajectory.h"
#include "trajectory_constant_acc.h"
#include "trajectory_pieced.h"

namespace Tyr::Soccer
{
struct VelocityProfile;

class Trajectory2D
{
protected:
    using TrajectoryPiece1D = TrajectoryConstantAcc<float>;
    using Trajectory1D = TrajectoryPieced<float, TrajectoryPiece1D>;

    Trajectory1D m_trajectory_x;
    Trajectory1D m_trajectory_y;

    static float velChangeToZero(float s0, float v0, float aMax);
    //    Velocity
    //    ^
    //    |    v1
    //    |    /\
    //    |   /  \
    //    |  /    \
    // v0 | /      \
    //    |/        \
    //    +-----------> Time
    //    0    t1   t1+t2
    // this return the pos at (t1+t2)
    static float velTriToZero(float s0, float v0, float v1, float aMax);
    static Trajectory1D calcTri(float s0, float v0, float s2, float a);
    static Trajectory1D calcTrapz(float s0, float v0, float v1, float s3, float aMax);
    static Trajectory1D makeBangBangTrajectory1D(float initialPos, float finalPos, float initialVel, float maxVel, float maxAcc);

public:
    Trajectory2D() = default;

    static Trajectory2D makeBangBangTrajectory(
        Common::Vec2 p0, Common::Vec2 v0,
        Common::Vec2 target, const VelocityProfile &profile);

    Common::Vec2 getPosition(const float t) const
    {
        return {m_trajectory_x.getPosition(t), m_trajectory_y.getPosition(t)};
    }

    Common::Vec2 getVelocity(const float t) const
    {
        return {m_trajectory_x.getVelocity(t), m_trajectory_y.getVelocity(t)};
    }

    Common::Vec2 getAcceleration(const float t) const
    {
        return {m_trajectory_x.getAcceleration(t), m_trajectory_y.getAcceleration(t)};
    }

    float getStartTime() const
    {
        return std::max(m_trajectory_x.getStartTime(), m_trajectory_y.getStartTime());
    }

    float getEndTime() const
    {
        return std::min(m_trajectory_x.getEndTime(), m_trajectory_y.getEndTime());
    }

    float getDuration() const
    {
        return std::max(0.0f, getEndTime() - getStartTime());
    }

    void draw(const Common::Color color) const
    {
        for (float t = getStartTime(); t < getEndTime(); t += 0.1f)
        {
            Common::debug().draw(getPosition(t), color);
        }
    }
};
} // namespace Tyr::Soccer
