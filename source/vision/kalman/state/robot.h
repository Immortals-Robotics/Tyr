#pragma once

namespace Tyr::Vision::Filter
{
class RobotState : public Kalman::Vector<float, 5>
{
public:
    KALMAN_VECTOR(RobotState, float, 5)

    explicit RobotState(const Common::Vec2 t_pos, const Common::Angle t_angle)
    {
        x() = t_pos.x;
        y() = t_pos.y;
        vx() = 0.f;
        vy() = 0.f;
        theta() = t_angle.deg();
    }

    static constexpr size_t X = 0;
    static constexpr size_t Y = 1;
    static constexpr size_t VX = 2;
    static constexpr size_t VY = 3;
    static constexpr size_t THETA = 4;

    float x()       const { return (*this)[ X ]; }
    float y()       const { return (*this)[ Y ]; }
    float vx()       const { return (*this)[ VX ]; }
    float vy()       const { return (*this)[ VY ]; }
    float theta()   const { return (*this)[ THETA ]; }

    float& x()      { return (*this)[ X ]; }
    float& y()      { return (*this)[ Y ]; }
    float& vx()      { return (*this)[ VX ]; }
    float& vy()      { return (*this)[ VY ]; }
    float& theta()  { return (*this)[ THETA ]; }
};
} // namespace Tyr::Vision
