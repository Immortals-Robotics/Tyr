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

    Common::Vec2 position() const
    {
        return {x(), y()};
    }

    Common::Vec2 velocity() const
    {
        return {vx(), vy()};
    }

    void setPosition(const Common::Vec2& position)
    {
        x() = position.x;
        y() = position.y;
    }

    void setVelocity(const Common::Vec2& velocity)
    {
        vx() = velocity.x;
        vy() = velocity.y;
    }
};
} // namespace Tyr::Vision
