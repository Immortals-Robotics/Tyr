#pragma once

#include "parabolic.h"

namespace Tyr::Soccer
{
class Trajectory
{
public:
    Parabolic acc;
    Parabolic dec;
    Parabolic stopped;

    static Trajectory MakeTrajectory(const Common::RobotState &state, const Common::Vec2 &a_acc, const float a_dec,
                                     const float a_dt);
    static Trajectory MakeOpponentTrajectory(const Common::RobotState &state, const float a_dec);
};
} // namespace Tyr::Soccer
