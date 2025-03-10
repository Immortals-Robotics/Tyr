#pragma once

#include "../state/robot.h"

namespace Tyr::Vision::Filter
{
class RobotSystemModel : public Kalman::LinearizedSystemModel<RobotState>
{
public:

    RobotSystemModel()
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

        // TODO: update based on the state velocity
        x_.x() = x.x();
        x_.y() = x.y();

        x_.theta() = x.theta();

        return x_;
    }
};
} // namespace Tyr::Vision
