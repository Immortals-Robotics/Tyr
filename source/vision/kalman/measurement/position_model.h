#pragma once

#include "../state/robot.h"
#include "position.h"

namespace Tyr::Vision::Filter
{
class PositionMeasurementModel : public Kalman::LinearizedMeasurementModel<RobotState, PositionMeasurement>
{
public:
    PositionMeasurementModel()
    {
        this->V.setIdentity();

        this->H.setZero();
        this->H(PositionMeasurement::X, RobotState::X) = 1.0;
        this->H(PositionMeasurement::Y, RobotState::Y) = 1.0;
    }

    PositionMeasurement h(const RobotState &x) const override
    {
        PositionMeasurement measurement;

        measurement.x() = x.x();
        measurement.y() = x.y();

        return measurement;
    }
};
} // namespace Tyr::Vision
