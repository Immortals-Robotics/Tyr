#pragma once

#include "../control.h"
#include "../state/robot.h"

namespace Tyr::Vision::Filter
{
class ControlledRobotSystemModel : public Kalman::LinearizedSystemModel<RobotState, Control>
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
        x_.theta() = x.theta() + u.dtheta() * dt;

        return x_;
    }

    void updateJacobians( const RobotState& x, const Control& u )override
    {
        (void)x;
        (void)u;

        // F = df/dx (Jacobian of state transition w.r.t. the state)
        this->F.setZero();

        // Position updates w.r.t. position (identity)
        this->F(RobotState::X, RobotState::X) = 1;
        this->F(RobotState::Y, RobotState::Y) = 1;

        // Position updates w.r.t. velocity (scaled by dt)
        const float dt = 1.f / Common::config().vision.vision_frame_rate;
        this->F(RobotState::X, RobotState::VX) = dt;
        this->F(RobotState::Y, RobotState::VY) = dt;

        // Velocity updates w.r.t. velocity (identity)
        this->F(RobotState::VX, RobotState::VX) = 1;
        this->F(RobotState::VY, RobotState::VY) = 1;

        // Orientation updates w.r.t. orientation
        this->F(RobotState::THETA, RobotState::THETA) = 1;

        // W = df/dw (Jacobian of state transition w.r.t. the noise)
        this->W.setIdentity();
    }
};
} // namespace Tyr::Vision
