#pragma once

namespace Tyr::Vision
{
// @brief Measurement vector measuring the robot position
class PositionMeasurement : public Kalman::Vector<float, 2>
{
public:
    KALMAN_VECTOR(PositionMeasurement, float, 2)

    //! X-position
    static constexpr size_t X = 0;
    //! Y-Position
    static constexpr size_t Y = 1;

    float x()       const { return (*this)[ X ]; }
    float y()       const { return (*this)[ Y ]; }

    float& x()      { return (*this)[ X ]; }
    float& y()      { return (*this)[ Y ]; }
};
} // namespace Tyr::Vision
