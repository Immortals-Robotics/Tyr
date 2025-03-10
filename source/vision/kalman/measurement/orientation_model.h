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
        this->H.setIdentity();
        this->V.setIdentity();
    }

    OrientationMeasurement h(const RobotState &x) const override
    {
        OrientationMeasurement measurement;
        measurement.theta() = x.theta();
        return measurement;
    }
};
} // namespace Tyr::Vision
