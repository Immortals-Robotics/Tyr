#pragma once

#include "../state.h"
#include "orientation.h"

namespace Tyr::Vision
{
/**
 * @brief Measurement model for measuring orientation of a 3DOF robot
 *
 * This is the measurement model for measuring the orientation of our
 * planar robot. This could be realized by a compass / magnetometer-sensor.
 *
 * @param CovarianceBase Class template to determine the covariance representation
 *                       (as covariance matrix (StandardBase) or as lower-triangular
 *                       coveriace square root (SquareRootBase))
 */
template<template<class> class CovarianceBase = Kalman::StandardBase>
class OrientationMeasurementModel : public Kalman::LinearizedMeasurementModel<State, OrientationMeasurement, CovarianceBase>
{
public:
    OrientationMeasurementModel()
    {
        // Setup jacobians. As these are static, we can define them once
        // and do not need to update them dynamically
        this->H.setIdentity();
        this->V.setIdentity();
    }

    /**
     * @brief Definition of (possibly non-linear) measurement function
     *
     * This function maps the system state to the measurement that is expected
     * to be received from the sensor assuming the system is currently in the
     * estimated state.
     *
     * @param [in] x The system state in current time-step
     * @returns The (predicted) sensor measurement for the system state
     */
    OrientationMeasurement h(const State& x) const
    {
        OrientationMeasurement measurement;

        // Measurement is given by the actual robot orientation
        measurement.theta() = x.theta();

        return measurement;
    }
};
} // namespace Tyr::Vision
