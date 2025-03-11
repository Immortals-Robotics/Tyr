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
        this->H(OrientationMeasurement::THETA_COS, RobotState::THETA_COS) = 1.f;
        this->H(OrientationMeasurement::THETA_SIN, RobotState::THETA_SIN) = 1.f;
    }

    OrientationMeasurement h(const RobotState &x) const override
    {
        OrientationMeasurement measurement{};
        measurement.theta_cos() = x.theta_cos();
        measurement.theta_sin() = x.theta_sin();
        return measurement;
    }
};
} // namespace Tyr::Vision
