#pragma once

#include "../state/robot.h"

namespace Tyr::Vision::Filter
{
class RobotSystemModel : public Kalman::SystemModel<RobotState, Kalman::Vector<float, 0>, Kalman::SquareRootBase>
{
public:

    RobotState f(const RobotState &x, const Control &u) const override
    {
        (void)u;

        //! Predicted state vector after transition
        RobotState x_;

        const float dt = 1.f / Common::config().vision.vision_frame_rate;

        // Position update based on velocity
        x_.x() = x.x() + x.vx() * dt;
        x_.y() = x.y() + x.vy() * dt;

        // Velocity remains unchanged (assuming constant velocity model)
        x_.vx() = x.vx();
        x_.vy() = x.vy();

        // Orientation remains unchanged (no control over it)
        x_.theta() = x.theta();

        return x_;
    }
};
} // namespace Tyr::Vision
