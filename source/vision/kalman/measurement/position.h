#pragma once

namespace Tyr::Vision::Filter
{
class PositionMeasurement : public Kalman::Vector<float, 2>
{
public:
    KALMAN_VECTOR(PositionMeasurement, float, 2)

    explicit PositionMeasurement(const Common::Vec2 t_pos)
    {
        setZero();
        x() = t_pos.x;
        y() = t_pos.y;
    }

    static constexpr size_t X = 0;
    static constexpr size_t Y = 1;

    float x()   const { return (*this)[ X ]; }
    float y()   const { return (*this)[ Y ]; }

    float& x()  { return (*this)[ X ]; }
    float& y()  { return (*this)[ Y ]; }
};
} // namespace Tyr::Vision
