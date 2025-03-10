#pragma once

#include "control.h"
#include "state.h"

namespace Tyr::Vision
{
/**
 * @brief System model for a simple planar 3DOF robot
 *
 * This is the system model defining how our robot moves from one
 * time-step to the next, i.e. how the system state evolves over time.
 *
 * @param CovarianceBase Class template to determine the covariance representation
 *                       (as covariance matrix (StandardBase) or as lower-triangular
 *                       coveriace square root (SquareRootBase))
 */
template<template<class> class CovarianceBase = Kalman::StandardBase>
class SystemModel : public Kalman::LinearizedSystemModel<State, Control, CovarianceBase>
{
public:
    /**
     * @brief Definition of (non-linear) state transition function
     *
     * This function defines how the system state is propagated through time,
     * i.e. it defines in which state \f$\hat{x}_{k+1}\f$ is system is expected to
     * be in time-step \f$k+1\f$ given the current state \f$x_k\f$ in step \f$k\f$ and
     * the system control input \f$u\f$.
     *
     * @param [in] x The system state in current time-step
     * @param [in] u The control vector input
     * @returns The (predicted) system state in the next time-step
     */
    State f(const State& x, const Control& u) const
    {
        //! Predicted state vector after transition
        State x_;

        // Update position using control input velocities
        x_.x() = x.x() + u.vx();
        x_.y() = x.y() + u.vy();

        // Update orientation
        x_.theta() = x.theta() + u.dtheta();

        // Return transitioned state vector
        return x_;
    }

protected:
    /**
     * @brief Update jacobian matrices for the system state transition function using current state
     *
     * This will re-compute the (state-dependent) elements of the jacobian matrices
     * to linearize the non-linear state transition function \f$f(x,u)\f$ around the
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
    void updateJacobians( const State& x, const Control& u )
    {
        // F = df/dx (Jacobian of state transition w.r.t. the state)
        this->F.setZero();

        // partial derivative of x.x() w.r.t. x.x()
        this->F( State::X, State::X ) = 1;

        // partial derivative of x.y() w.r.t. x.y()
        this->F( State::Y, State::Y ) = 1;

        // partial derivative of x.theta() w.r.t. x.theta()
        this->F( State::THETA, State::THETA ) = 1;

        // W = df/dw (Jacobian of state transition w.r.t. the noise)
        this->W.setIdentity();
        // TODO: more sophisticated noise modelling
        //       i.e. The noise affects the the direction in which we move as
        //       well as the velocity (i.e. the distance we move)
    }
};
} // namespace Tyr::Vision
