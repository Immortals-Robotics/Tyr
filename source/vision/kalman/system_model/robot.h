#pragma once

#include "../state/robot.h"

namespace Tyr::Vision::Filter
{
class RobotSystemModel : public Kalman::LinearizedSystemModel<RobotState>
{
public:

    RobotState f(const RobotState &state, const Control &control) const override
    {
        (void)control;

        //! Predicted state vector after transition
        RobotState next_state{};
        next_state.setZero();

        const float dt = 1.f / Common::config().vision.vision_frame_rate;

        // Position update based on velocity
        next_state.x() = state.x() + state.vx() * dt;
        next_state.y() = state.y() + state.vy() * dt;

        // Velocity remains unchanged (assuming constant velocity model)
        next_state.vx() = state.vx();
        next_state.vy() = state.vy();

        // Orientation update based on omega
        next_state.theta_cos() = state.theta_cos() + state.omega() * (-state.theta_sin()) * dt;
        next_state.theta_sin() = state.theta_sin() + state.omega() * state.theta_cos() * dt;

        // Omega remains unchanged (assuming constant velocity model)
        next_state.omega() = state.omega();

        return next_state;
    }

    // note: these are only used in linear kalmans (extended)
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
        this->F(RobotState::THETA_COS, RobotState::THETA_COS) = 1;
        this->F(RobotState::THETA_SIN, RobotState::THETA_SIN) = 1;

        // Orientation components w.r.t each-other
        this->F(RobotState::THETA_COS, RobotState::THETA_SIN) = -x.omega() * dt;
        this->F(RobotState::THETA_SIN, RobotState::THETA_COS) = x.omega() * dt;

        // Orientation updates w.r.t. omega (dt factor)
        this->F(RobotState::THETA_COS, RobotState::OMEGA) = -x.theta_sin() * dt;
        this->F(RobotState::THETA_SIN, RobotState::OMEGA) = x.theta_cos() * dt;

        // Omega updates
        this->F(RobotState::OMEGA, RobotState::OMEGA) = 1;

        // W = df/dw (Jacobian of state transition w.r.t. noise)
        this->W.setIdentity();
    }
};
} // namespace Tyr::Vision
