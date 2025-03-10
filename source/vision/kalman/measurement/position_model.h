#pragma once

#include "../state/robot.h"
#include "position.h"

namespace Tyr::Vision::Filter
{
class PositionMeasurementModel : public Kalman::MeasurementModel<RobotState, PositionMeasurement, Kalman::SquareRootBase>
{
public:
    PositionMeasurementModel() = default;

    PositionMeasurement h(const RobotState &x) const override
    {
        PositionMeasurement measurement;

        measurement.x() = x.x();
        measurement.y() = x.y();

        return measurement;
    }
};
} // namespace Tyr::Vision
