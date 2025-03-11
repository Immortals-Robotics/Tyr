#pragma once

namespace Tyr::Vision::Filter
{
class RobotState : public Kalman::Vector<float, 6>
{
public:
    KALMAN_VECTOR(RobotState, float, 6)

    static constexpr size_t X = 0;
    static constexpr size_t Y = 1;
    static constexpr size_t VX = 2;
    static constexpr size_t VY = 3;
    static constexpr size_t THETA_COS = 4;
    static constexpr size_t THETA_SIN = 5;

    float x()         const { return (*this)[ X ]; }
    float y()         const { return (*this)[ Y ]; }
    float vx()        const { return (*this)[ VX ]; }
    float vy()        const { return (*this)[ VY ]; }
    float theta_cos() const { return (*this)[ THETA_COS ]; }
    float theta_sin() const { return (*this)[ THETA_SIN ]; }

    float& x()         { return (*this)[ X ]; }
    float& y()         { return (*this)[ Y ]; }
    float& vx()        { return (*this)[ VX ]; }
    float& vy()        { return (*this)[ VY ]; }
    float& theta_cos() { return (*this)[ THETA_COS ]; }
    float& theta_sin() { return (*this)[ THETA_SIN ]; }
};
} // namespace Tyr::Vision
