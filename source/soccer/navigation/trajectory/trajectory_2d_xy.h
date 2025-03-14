#pragma once

#include "trajectory_base.h"
#include "trajectory_1d.h"
#include "piece/piece_2d.h"

#include "../obstacle/map.h"

namespace Tyr::Soccer
{
struct VelocityProfile;

class Trajectory2DXY final : public TrajectoryBase<Common::Vec2>
{
protected:
    Trajectory1D m_trajectory_x;
    Trajectory1D m_trajectory_y;

public:
    Trajectory2DXY() = default;

    static Trajectory2DXY makeBangBangTrajectory(Common::Vec2 p0, Common::Vec2 v0, Common::Vec2 target, const VelocityProfile &profile);

    // generates a piece to be sent to the robot for a control period of dt
    TrajectoryPiece2D getCommandPiece(const float dt) const
    {
        const float t0 = getStartTime();
        const float t1 = t0 + dt;

        TrajectoryPiece2D cmd_piece{};
        cmd_piece.acc     = getAcceleration(t0);
        cmd_piece.v_start = getVelocity(t0);
        cmd_piece.p_start = getPosition(t0);
        cmd_piece.t_start = t0;
        cmd_piece.t_end   = t1;

        return cmd_piece;
    }
    Common::Vec2 getPosition(const float t) const override
    {
        return {m_trajectory_x.getPosition(t), m_trajectory_y.getPosition(t)};
    }

    Common::Vec2 getVelocity(const float t) const override
    {
        return {m_trajectory_x.getVelocity(t), m_trajectory_y.getVelocity(t)};
    }

    Common::Vec2 getAcceleration(const float t) const override
    {
        return {m_trajectory_x.getAcceleration(t), m_trajectory_y.getAcceleration(t)};
    }

    float getStartTime() const override
    {
        return std::min(m_trajectory_x.getStartTime(), m_trajectory_y.getStartTime());
    }

    float getEndTime() const override
    {
        return std::max(m_trajectory_x.getEndTime(), m_trajectory_y.getEndTime());
    }

    // TODO: this duplicates the one in trajectory_2d
    bool hasCollision(const TrajectoryBase &other, float r, float look_ahead = 3.0f, float step_t = 0.1f) const override
    {
        const float t_start = std::max(this->getStartTime(), other.getStartTime());
        const float t_end_raw = std::min(this->getEndTime(), other.getEndTime());
        const float t_end = std::min(t_end_raw, t_start + look_ahead);

        for (float t = t_start; t < t_end; t += step_t)
        {
            const Common::Vec2 pos = getPosition(t);
            const Common::Vec2 other_pos = other.getPosition(t);

            if (pos.distanceTo(other_pos) <= r)
            {
                return true;
            }
        }

        return false;
    }

    // TODO: this duplicates the one in trajectory_2d
    bool hasCollision(const ObstacleMap &map, float look_ahead = 3.0f, float step_t = 0.1f) const override
    {
        const float t_end = std::min(getEndTime(), getStartTime() + look_ahead);

        for (float t = getStartTime(); t < t_end; t += step_t)
        {
            const Common::Vec2 pos = getPosition(t);

            if (map.inside(pos))
            {
                return true;
            }
        }

        return false;
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
