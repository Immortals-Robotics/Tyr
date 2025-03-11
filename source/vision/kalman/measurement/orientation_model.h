#pragma once

#include "../state/robot.h"
#include "orientation.h"

namespace Tyr::Vision::Filter
{
class OrientationMeasurementModel : public Kalman::LinearizedMeasurementModel<RobotState, OrientationMeasurement>
{
public:
    OrientationMeasurementModel()
    {
        // note: these are only used in linear kalmans (extended)
        this->V.setIdentity();

        this->H.setZero();
        this->H(OrientationMeasurement::THETA, RobotState::THETA) = 1.0;
    }

    OrientationMeasurement h(const RobotState &x) const override
    {
        OrientationMeasurement measurement;
        measurement.theta() = x.theta();
        return measurement;
    }
};
} // namespace Tyr::Vision
