#pragma once

namespace Tyr::Vision::Filter
{
class BallState : public Kalman::Vector<float, 2>
{
public:
    KALMAN_VECTOR(BallState, float, 2)

    static constexpr size_t X = 0;
    static constexpr size_t Y = 1;

    float x()       const { return (*this)[ X ]; }
    float y()       const { return (*this)[ Y ]; }

    float& x()      { return (*this)[ X ]; }
    float& y()      { return (*this)[ Y ]; }
};
} // namespace Tyr::Vision
