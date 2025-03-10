#pragma once

#include "../state/robot.h"
#include "orientation.h"

namespace Tyr::Vision::Filter
{
class OrientationMeasurementModel : public Kalman::MeasurementModel<RobotState, OrientationMeasurement, Kalman::SquareRootBase>
{
public:
    OrientationMeasurementModel() = default;

    OrientationMeasurement h(const RobotState &x) const override
    {
        OrientationMeasurement measurement;
        measurement.theta() = x.theta();
        return measurement;
    }
};
} // namespace Tyr::Vision
