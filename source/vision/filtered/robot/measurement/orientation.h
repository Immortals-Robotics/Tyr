#pragma once

namespace Tyr::Vision
{
// @brief Measurement vector measuring an orientation (i.e. by using a compass)
class OrientationMeasurement : public Kalman::Vector<float, 1>
{
public:
    KALMAN_VECTOR(OrientationMeasurement, float, 1)

    //! Orientation
    static constexpr size_t THETA = 0;

    float theta()  const { return (*this)[ THETA ]; }
    float& theta() { return (*this)[ THETA ]; }
};

} // namespace Tyr::Vision
