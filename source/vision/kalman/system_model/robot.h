#pragma once

#include "../state/robot.h"

namespace Tyr::Vision::Filter
{
class RobotSystemModel : public Kalman::LinearizedSystemModel<RobotState>
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

    void updateJacobians( const RobotState& x, const Control& u )override
    {
        (void)x;
        (void)u;

        // Compute dt dynamically
        const float dt = 1.f / Common::config().vision.vision_frame_rate;

        // F = df/dx (Jacobian of state transition w.r.t. the state)
        this->F.setZero();

        // Position updates w.r.t. position (identity)
        this->F(RobotState::X, RobotState::X) = 1;
        this->F(RobotState::Y, RobotState::Y) = 1;

        // Position updates w.r.t. velocity (dt factor)
        this->F(RobotState::X, RobotState::VX) = dt;
        this->F(RobotState::Y, RobotState::VY) = dt;

        // Velocity updates
        this->F(RobotState::VX, RobotState::VX) = 1;
        this->F(RobotState::VY, RobotState::VY) = 1;

        // Orientation updates (remains unchanged)
        this->F(RobotState::THETA, RobotState::THETA) = 1;

        // W = df/dw (Jacobian of state transition w.r.t. noise)
        this->W.setIdentity();
    }
};
} // namespace Tyr::Vision
