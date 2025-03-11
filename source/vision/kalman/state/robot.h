#pragma once

namespace Tyr::Vision::Filter
{
class RobotState : public Kalman::Vector<float, 7>
{
public:
    KALMAN_VECTOR(RobotState, float, 7)

    static constexpr size_t X = 0;
    static constexpr size_t Y = 1;
    static constexpr size_t VX = 2;
    static constexpr size_t VY = 3;
    static constexpr size_t THETA_COS = 4;
    static constexpr size_t THETA_SIN = 5;
    static constexpr size_t OMEGA = 6;

    float x()         const { return (*this)[ X ]; }
    float y()         const { return (*this)[ Y ]; }
    float vx()        const { return (*this)[ VX ]; }
    float vy()        const { return (*this)[ VY ]; }
    float theta_cos() const { return (*this)[ THETA_COS ]; }
    float theta_sin() const { return (*this)[ THETA_SIN ]; }
    float omega()     const { return (*this)[ OMEGA ]; }

    float& x()         { return (*this)[ X ]; }
    float& y()         { return (*this)[ Y ]; }
    float& vx()        { return (*this)[ VX ]; }
    float& vy()        { return (*this)[ VY ]; }
    float& theta_cos() { return (*this)[ THETA_COS ]; }
    float& theta_sin() { return (*this)[ THETA_SIN ]; }
    float& omega()     { return (*this)[ OMEGA ]; }
};
} // namespace Tyr::Vision
