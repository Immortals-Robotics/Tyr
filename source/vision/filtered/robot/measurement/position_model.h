#pragma once

#include "../state.h"
#include "position.h"

namespace Tyr::Vision
{
/**
 * @brief Measurement model for measuring the position of the robot
 *
 * This is the measurement model for measuring the position of the robot.
 *
 * @param CovarianceBase Class template to determine the covariance representation
 *                       (as covariance matrix (StandardBase) or as lower-triangular
 *                       coveriace square root (SquareRootBase))
 */
template<template<class> class CovarianceBase = Kalman::StandardBase>
class PositionMeasurementModel : public Kalman::LinearizedMeasurementModel<State, PositionMeasurement, CovarianceBase>
{
public:
    PositionMeasurementModel()
    {
        // Setup noise jacobian. As this one is static, we can define it once
        // and do not need to update it dynamically
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
    PositionMeasurement h(const State& x) const
    {
        PositionMeasurement measurement;

        measurement.x() = x.x();
        measurement.y() = x.y();

        return measurement;
    }

protected:

    /**
     * @brief Update Jacobian matrices for the measurement function using the current state
     *
     * Since the measurement function is now linear (just extracting x and y),
     * the Jacobian is simply an identity matrix.
     *
     * @param x The current system state
     */
    void updateJacobians( const State& x )
    {
        // H = dh/dx (Jacobian of measurement function w.r.t. the state)
        this->H.setZero();

        // The measurement directly observes (x, y), so the Jacobian is:
        this->H(PositionMeasurement::X, State::X) = 1.0;
        this->H(PositionMeasurement::Y, State::Y) = 1.0;
    }
};
} // namespace Tyr::Vision
