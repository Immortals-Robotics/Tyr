#pragma once

namespace Tyr::Vision::Filter
{
class OrientationMeasurement : public Kalman::Vector<float, 1>
{
public:
    KALMAN_VECTOR(OrientationMeasurement, float, 1)

    explicit OrientationMeasurement(const Common::Angle t_angle)
    {
        theta() = t_angle.deg();
    }

    static constexpr size_t THETA = 0;

    float theta()  const { return (*this)[ THETA ]; }
    float& theta() { return (*this)[ THETA ]; }
};

} // namespace Tyr::Vision
