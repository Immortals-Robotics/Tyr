#pragma once

#include "../state/ball.h"

namespace Tyr::Vision::Filter
{
class BallSystemModel : public Kalman::LinearizedSystemModel<BallState>
{
public:
    BallState f(const BallState &state, const Control &u) const override
    {
        (void)u;

        //! Predicted state vector after transition
        BallState new_state{};
        new_state.setZero();

        const float dt = 1.f / Common::config().vision.vision_frame_rate;

        // Position update based on velocity
        new_state.x() = state.x() + state.vx() * dt;
        new_state.y() = state.y() + state.vy() * dt;

        // Velocity remains unchanged (assuming constant velocity model)
        new_state.vx() = state.vx();
        new_state.vy() = state.vy();

        return new_state;
    }

    // note: these are only used in linear kalmans (extended)
    void updateJacobians( const BallState& x, const Control& u )override
    {
        (void)x;
        (void)u;

        // Compute dt dynamically
        const float dt = 1.f / Common::config().vision.vision_frame_rate;

        // F = df/dx (Jacobian of state transition w.r.t. the state)
        this->F.setZero();

        // Position updates w.r.t. position (identity)
        this->F(BallState::X, BallState::X) = 1;
        this->F(BallState::Y, BallState::Y) = 1;

        // Position updates w.r.t. velocity (dt factor)
        this->F(BallState::X, BallState::VX) = dt;
        this->F(BallState::Y, BallState::VY) = dt;

        // Velocity updates
        this->F(BallState::VX, BallState::VX) = 1;
        this->F(BallState::VY, BallState::VY) = 1;

        // W = df/dw (Jacobian of state transition w.r.t. noise)
        this->W.setIdentity();
    }
};
} // namespace Tyr::Vision
