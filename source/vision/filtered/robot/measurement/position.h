#pragma once

namespace Tyr::Vision
{
// @brief Measurement vector measuring the robot position
class PositionMeasurement : public Kalman::Vector<float, 2>
{
public:
    KALMAN_VECTOR(PositionMeasurement, float, 2)

    //! Distance to landmark 1
    static constexpr size_t D1 = 0;

    //! Distance to landmark 2
    static constexpr size_t D2 = 1;

    float d1()       const { return (*this)[ D1 ]; }
    float d2()       const { return (*this)[ D2 ]; }

    float& d1()      { return (*this)[ D1 ]; }
    float& d2()      { return (*this)[ D2 ]; }
};
} // namespace Tyr::Vision
