#pragma once

#include "trajectory_pieced.h"
#include "piece/piece_2d.h"

namespace Tyr::Soccer
{
struct VelocityProfile;

class Trajectory2D : public TrajectoryPieced<Common::Vec2, TrajectoryPiece2D>
{
public:
    Trajectory2D() = default;

    // makes a trajectory that consists of:
    // 1- execute the cmd_piece
    // 2- decelerating with a_dec until full stop
    // 3- being at full stop till the end of time (literally)
    static Trajectory2D makeDssTrajectory(const TrajectoryPiece2D& cmd_piece, float a_dec);

    // makes a trajectory that consists of:
    //  - skip the cmd_piece
    // 1- decelerating with a_dec until full stop
    // 2- being at full stop till the end of time (literally)
    static Trajectory2D makeStopDssTrajectory(const TrajectoryPiece2D& cmd_piece, float a_dec);

    static Trajectory2D makeRobotTrajectory(Common::Vec2 p0, Common::Vec2 v0, Common::Vec2 target, const VelocityProfile &profile);

    static Trajectory2D makeBangBangTrajectory(Common::Vec2 p0, Common::Vec2 v0, Common::Vec2 target,
                                               const VelocityProfile &profile);

    std::pair<bool, float> hasCollision(const TrajectoryBase &other, float r, float look_ahead = 3.0f,
                                        float step_t = 0.1f) const override;
    std::pair<bool, float> hasCollision(const ObstacleMap &map, float look_ahead = 3.0f,
                                        float step_t = 0.1f) const override;

    std::pair<bool, float> reachFree(const ObstacleMap &map, float look_ahead, float step_t) const override;

    void draw(const Common::Color color) const override
    {
        for (float t = getStartTime(); t < getEndTime(); t += 0.1f)
        {
            Common::debug().draw(getPosition(t), color);
        }
    }

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
};
} // namespace Tyr::Soccer
