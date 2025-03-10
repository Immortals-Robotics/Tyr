#pragma once

namespace Tyr::Vision::Filter
{
class RobotState : public Kalman::Vector<float, 3>
{
public:
    KALMAN_VECTOR(RobotState, float, 3)

    static constexpr size_t X = 0;
    static constexpr size_t Y = 1;
    static constexpr size_t THETA = 2;

    float x()       const { return (*this)[ X ]; }
    float y()       const { return (*this)[ Y ]; }
    float theta()   const { return (*this)[ THETA ]; }

    float& x()      { return (*this)[ X ]; }
    float& y()      { return (*this)[ Y ]; }
    float& theta()  { return (*this)[ THETA ]; }
};
} // namespace Tyr::Vision
