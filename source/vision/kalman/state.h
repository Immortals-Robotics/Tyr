#pragma once

namespace Tyr::Vision
{
// @brief System state vector-type for a 3DOF planar robot
//
// This is a system state for a very simple planar robot that
// is characterized by its (x,y)-Position and angular orientation.
class State : public Kalman::Vector<float, 3>
{
public:
    KALMAN_VECTOR(State, float, 3)

    //! X-position
    static constexpr size_t X = 0;
    //! Y-Position
    static constexpr size_t Y = 1;
    //! Orientation
    static constexpr size_t THETA = 2;

    float x()       const { return (*this)[ X ]; }
    float y()       const { return (*this)[ Y ]; }
    float theta()   const { return (*this)[ THETA ]; }

    float& x()      { return (*this)[ X ]; }
    float& y()      { return (*this)[ Y ]; }
    float& theta()  { return (*this)[ THETA ]; }
};
} // namespace Tyr::Vision
