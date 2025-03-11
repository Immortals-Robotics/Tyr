#pragma once

namespace Tyr::Vision::Filter
{
class OrientationMeasurement : public Kalman::Vector<float, 2>
{
public:
    KALMAN_VECTOR(OrientationMeasurement, float, 2)

    explicit OrientationMeasurement(const Common::Angle t_angle)
    {
        setZero();
        theta_cos() = t_angle.cos();
        theta_sin() = t_angle.sin();
    }

    static constexpr size_t THETA_COS = 0;
    static constexpr size_t THETA_SIN = 1;

    float theta_cos()  const { return (*this)[ THETA_COS ]; }
    float theta_sin()  const { return (*this)[ THETA_SIN ]; }

    float& theta_cos() { return (*this)[ THETA_COS ]; }
    float& theta_sin() { return (*this)[ THETA_SIN ]; }
};
} // namespace Tyr::Vision
