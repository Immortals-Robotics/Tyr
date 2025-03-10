#pragma once

namespace Tyr::Vision::Filter
{
class Control : public Kalman::Vector<float, 3>
{
public:
    KALMAN_VECTOR(Control, float, 3)

    static constexpr size_t VX = 0;
    static constexpr size_t VY = 1;
    static constexpr size_t DTHETA = 2;

    float vx()      const { return (*this)[ VX ]; }
    float vy()      const { return (*this)[ VY ]; }
    float dtheta()  const { return (*this)[ DTHETA ]; }

    float& vx()     { return (*this)[ VX ]; }
    float& vy()     { return (*this)[ VY ]; }
    float& dtheta() { return (*this)[ DTHETA ]; }
};
} // namespace Tyr::Vision
