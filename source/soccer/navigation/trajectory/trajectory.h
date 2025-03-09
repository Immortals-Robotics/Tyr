#pragma once

#include "trajectory_piece.h"

namespace Tyr::Soccer
{
class Trajectory
{
public:
    TrajectoryPiece acc;
    TrajectoryPiece dec;
    TrajectoryPiece stopped;

    // makes a trajectory that consists of:
    // 1- accelerating with a_acc for a_dt
    // 2- decelerating with a_dec until full stop
    // 3- being at full stop till the end of time (literally)
    static Trajectory MakeTrajectory(const Common::RobotState &state, const Common::Vec2 &a_acc, float a_dec, float a_dt);

    // makes a trajectory that consists of:
    // 1- accelerating for 0s
    // 2- decelerating with a_dec until full stop
    // 3- being at full stop till the end of time (literally)
    static Trajectory MakeOpponentTrajectory(const Common::RobotState &state, float a_dec);
};
} // namespace Tyr::Soccer
