#pragma once

namespace Tyr::Vision
{
// TODO: this doesn't apply to a omni-directional robot

// @brief System control-input vector-type for a 3DOF planar robot
//
// This is the system control-input of a very simple planar robot that
// can control the velocity in its current direction as well as the
// change in direction.
class Control : public Kalman::Vector<float, 2>
{
public:
    KALMAN_VECTOR(Control, float, 2)

    //! Velocity
    static constexpr size_t V = 0;
    //! Angular Rate (Orientation-change)
    static constexpr size_t DTHETA = 1;

    float v()       const { return (*this)[ V ]; }
    float dtheta()  const { return (*this)[ DTHETA ]; }

    float& v()      { return (*this)[ V ]; }
    float& dtheta() { return (*this)[ DTHETA ]; }
};
} // namespace Tyr::Vision
