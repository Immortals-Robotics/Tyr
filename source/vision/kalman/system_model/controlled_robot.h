#pragma once

#include "control.h"
#include "../state/robot.h"

namespace Tyr::Vision::Filter
{
class ControlledRobotSystemModel : public Kalman::LinearizedSystemModel<RobotState, Control>
{
public:

    ControlledRobotSystemModel()
    {
        this->F.setZero();

        this->F( RobotState::X, RobotState::X ) = 1;
        this->F( RobotState::Y, RobotState::Y ) = 1;
        this->F( RobotState::THETA, RobotState::THETA ) = 1;

        this->W.setIdentity();
    }

    RobotState f(const RobotState &x, const Control &u) const override
    {
        RobotState x_;

        x_.x() = x.x() + u.vx();
        x_.y() = x.y() + u.vy();

        x_.theta() = x.theta() + u.dtheta();

        return x_;
    }
};
} // namespace Tyr::Vision
