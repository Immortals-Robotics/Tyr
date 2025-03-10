#pragma once

#include "../state.h"
#include "position.h"

namespace Tyr::Vision
{
// TODO: this maps distance to some landmarks as sensory input, to state position,
// we don't need this as we directly receive position from vision

/**
 * @brief Measurement model for measuring the position of the robot
 *        using two beacon-landmarks
 *
 * This is the measurement model for measuring the position of the robot.
 * The measurement is given by two landmarks in the space, whose positions are known.
 * The robot can measure the direct distance to both the landmarks, for instance
 * through visual localization techniques.
 *
 * @param CovarianceBase Class template to determine the covariance representation
 *                       (as covariance matrix (StandardBase) or as lower-triangular
 *                       coveriace square root (SquareRootBase))
 */
template<template<class> class CovarianceBase = Kalman::StandardBase>
class PositionMeasurementModel : public Kalman::LinearizedMeasurementModel<State, PositionMeasurement, CovarianceBase>
{
public:
    /**
     * @brief Constructor
     *
     * @param landmark1x The x-position of landmark 1
     * @param landmark1y The y-position of landmark 1
     * @param landmark2x The x-position of landmark 2
     * @param landmark2y The y-position of landmark 2
     */
    PositionMeasurementModel(float landmark1x, float landmark1y, float landmark2x, float landmark2y)
    {
        // Save landmark positions
        landmark1 << landmark1x, landmark1y;
        landmark2 << landmark2x, landmark2y;

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

        // Robot position as (x,y)-vector
        // This uses the Eigen template method to get the first 2 elements of the vector
        Kalman::Vector<float, 2> position = x.template head<2>();

        // Distance of robot to landmark 1
        Kalman::Vector<float, 2> delta1 = position - landmark1;
        measurement.d1() = std::sqrt( delta1.dot(delta1) );

        // Distance of robot to landmark 2
        Kalman::Vector<float, 2> delta2 = position - landmark2;
        measurement.d2() = std::sqrt( delta2.dot(delta2) );

        return measurement;
    }

protected:
    //! Position of landmark 1 given as (x,y)-measurement
    Kalman::Vector<float, 2> landmark1;

    //! Position of landmark 2 given as (x,y)-measurement
    Kalman::Vector<float, 2> landmark2;

protected:

    /**
     * @brief Update jacobian matrices for the system state transition function using current state
     *
     * This will re-compute the (state-dependent) elements of the jacobian matrices
     * to linearize the non-linear measurement function \f$h(x)\f$ around the
     * current state \f$x\f$.
     *
     * @note This is only needed when implementing a LinearizedSystemModel,
     *       for usage with an ExtendedKalmanFilter or SquareRootExtendedKalmanFilter.
     *       When using a fully non-linear filter such as the UnscentedKalmanFilter
     *       or its square-root form then this is not needed.
     *
     * @param x The current system state around which to linearize
     * @param u The current system control input
     */
    void updateJacobians( const State& x )
    {
        // H = dh/dx (Jacobian of measurement function w.r.t. the state)
        this->H.setZero();

        // Robot position as (x,y)-vector
        // This uses the Eigen template method to get the first 2 elements of the vector
        Kalman::Vector<float, 2> position = x.template head<2>();

        // Distance of robot to landmark 1
        Kalman::Vector<float, 2> delta1 = position - landmark1;

        // Distance of robot to landmark 2
        Kalman::Vector<float, 2> delta2 = position - landmark2;

        // Distances
        float d1 = std::sqrt( delta1.dot(delta1) );
        float d2 = std::sqrt( delta2.dot(delta2) );

        // partial derivative of meas.d1() w.r.t. x.x()
        this->H( PositionMeasurement::D1, State::X ) = delta1[0] / d1;
        // partial derivative of meas.d1() w.r.t. x.y()
        this->H( PositionMeasurement::D1, State::Y ) = delta1[1] / d1;

        // partial derivative of meas.d1() w.r.t. x.x()
        this->H( PositionMeasurement::D2, State::X ) = delta2[0] / d2;
        // partial derivative of meas.d1() w.r.t. x.y()
        this->H( PositionMeasurement::D2, State::Y ) = delta2[1] / d2;
    }
};
} // namespace Tyr::Vision
