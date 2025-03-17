#pragma once

#include "../control.h"
#include "../state/robot.h"

namespace Tyr::Vision::Filter
{
class ControlledRobotSystemModel : public Kalman::SystemModel<RobotState, Control>
{
public:

    RobotState f(const RobotState &state, const Control &control) const override
    {
        RobotState next_state{};
        next_state.setZero();

        const float dt = 1.f / Common::config().vision.vision_frame_rate;

        // Update position using the independent velocity components
        next_state.x() = state.x() + state.vx() * dt;
        next_state.y() = state.y() + state.vy() * dt;

        // Update velocity from control input
        next_state.vx() = control.vx();
        next_state.vy() = control.vy();

        // Update orientation separately
        // TODO:
        //next_state.theta() = state.theta() + control.dtheta() * dt;

        return next_state;
    }
};
} // namespace Tyr::Vision
