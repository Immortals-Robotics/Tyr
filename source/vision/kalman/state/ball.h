#pragma once

namespace Tyr::Vision::Filter
{
class BallState : public Kalman::Vector<float, 4>
{
public:
    KALMAN_VECTOR(BallState, float, 4)

    static constexpr size_t X = 0;
    static constexpr size_t Y = 1;
    static constexpr size_t VX = 2;
    static constexpr size_t VY = 3;

    float x()       const { return (*this)[ X ]; }
    float y()       const { return (*this)[ Y ]; }
    float vx()       const { return (*this)[ VX ]; }
    float vy()       const { return (*this)[ VY ]; }

    float& x()      { return (*this)[ X ]; }
    float& y()      { return (*this)[ Y ]; }
    float& vx()      { return (*this)[ VX ]; }
    float& vy()      { return (*this)[ VY ]; }
};
} // namespace Tyr::Vision
