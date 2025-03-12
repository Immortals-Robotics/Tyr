#pragma once

#include "../state/robot.h"
#include "position.h"

namespace Tyr::Vision::Filter
{
template<typename State>
class PositionMeasurementModel : public Kalman::LinearizedMeasurementModel<State, PositionMeasurement>
{
public:
    PositionMeasurementModel()
    {
        // note: these are only used in linear kalmans (extended)
        this->V.setIdentity();

        this->H.setZero();
        this->H(PositionMeasurement::X, RobotState::X) = 1.0;
        this->H(PositionMeasurement::Y, RobotState::Y) = 1.0;
    }

    PositionMeasurement h(const State &x) const override
    {
        PositionMeasurement measurement;
        measurement.setZero();

        measurement.x() = x.x();
        measurement.y() = x.y();

        return measurement;
    }
};
} // namespace Tyr::Vision
