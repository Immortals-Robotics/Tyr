#pragma once

namespace Tyr::Vision::Filter
{
/**
 * @brief System control-input vector-type for an omnidirectional 3DOF planar robot
 *
 * The control inputs are now independent velocities in x and y directions
 * and the change in orientation (`dtheta`).
 *
 */
class Control : public Kalman::Vector<float, 3>
{
public:
    KALMAN_VECTOR(Control, float, 3)

    //! Velocity in X direction
    static constexpr size_t VX = 0;
    //! Velocity in Y direction
    static constexpr size_t VY = 1;
    //! Angular Rate (Orientation change)
    static constexpr size_t DTHETA = 2;

    float vx()      const { return (*this)[ VX ]; }
    float vy()      const { return (*this)[ VY ]; }
    float dtheta()  const { return (*this)[ DTHETA ]; }

    float& vx()     { return (*this)[ VX ]; }
    float& vy()     { return (*this)[ VY ]; }
    float& dtheta() { return (*this)[ DTHETA ]; }
};
} // namespace Tyr::Vision
