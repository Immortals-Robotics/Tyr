#pragma once

#include "../control.h"
#include "../state/robot.h"

namespace Tyr::Vision::Filter
{
class ControlledRobotSystemModel : public Kalman::SystemModel<RobotState, Control>
{
public:

    RobotState f(const RobotState &x, const Control &u) const override
    {
        RobotState x_;

        const float dt = 1.f / Common::config().vision.vision_frame_rate;

        // Update position using the independent velocity components
        x_.x() = x.x() + x.vx() * dt;
        x_.y() = x.y() + x.vy() * dt;

        // Update velocity from control input
        x_.vx() = u.vx();
        x_.vy() = u.vy();

        // Update orientation separately
        // TODO:
        //x_.theta() = x.theta() + u.dtheta() * dt;

        return x_;
    }
};
} // namespace Tyr::Vision
