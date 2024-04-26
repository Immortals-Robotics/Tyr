#include "angle.h"

namespace Tyr::Common
{
static constexpr float kDeg2Rad = std::numbers::pi / 180.0;
static constexpr float kRad2Deg = 180.0 / std::numbers::pi;

float Angle::rad() const
{
    return m_deg * kDeg2Rad;
}

float Angle::deg() const
{
    return m_deg;
}

float Angle::sin() const
{
    return std::sin(rad());
}
float Angle::cos() const
{
    return std::cos(rad());
}

float Angle::tan() const
{
    return std::tan(rad());
}

void Angle::setDeg(const float deg)
{
    m_deg = deg;
    normalize();
}

void Angle::setRad(const float rad)
{
    setDeg(rad * kRad2Deg);
}

void Angle::normalize()
{
    m_deg = std::remainder(m_deg, 360.0f);
}

Angle Angle::operator+(const Angle ang) const
{
    return fromDeg(m_deg + ang.m_deg);
}

Angle Angle::operator-(const Angle ang) const
{
    return fromDeg(m_deg - ang.m_deg);
}

void Angle::operator+=(const Angle ang)
{
    setDeg(m_deg + ang.m_deg);
}

void Angle::operator-=(const Angle ang)
{
    setDeg(m_deg - ang.m_deg);
}

bool Angle::operator<(Angle ang) const
{
    return (ang - *this).deg() > 0;
}

bool Angle::operator>(Angle ang) const
{
    return (ang - *this).deg() < 0;
}

Angle Angle::operator*(float f) const
{
    return fromDeg(m_deg * f);
}

Angle Angle::operator/(float f) const
{
    return fromDeg(m_deg / f);
}

vec2 Angle::toUnitVec() const
{
    const float rad = this->rad();
    return {std::cos(rad), std::sin(rad)};
}

bool Angle::isBetween(const Angle a, const Angle b) const
{
    // convert all angles to 0..360
    const float ang_360 = std::abs(std::fmod(m_deg, 360.0));
    const float a_360   = std::abs(std::fmod(a.m_deg, 360.0));
    const float b_360   = std::abs(std::fmod(b.m_deg, 360.0));

    const float ang_min_360 = std::min(a_360, b_360);
    const float ang_max_360 = std::max(a_360, b_360);

    return ang_min_360 < ang_360 && ang_360 < ang_max_360;
}

Angle Angle::average(const Angle a, const Angle b)
{
    const vec2 average_vector = (a.toUnitVec() + b.toUnitVec()) / 2.0f;
    return to_angle(average_vector);
}

Angle Angle::fromDeg(const float deg)
{
    Angle ang;
    ang.setDeg(deg);
    return ang;
}

Angle Angle::fromRad(const float rad)
{
    Angle ang;
    ang.setRad(rad);
    return ang;
}

Angle angle_with(const vec2 &a, const vec2 &b)
{
    return to_angle(b - a);
}

Angle to_angle(const vec2 &v)
{
    if (v.y == 0.0 && v.x == 0.0)
    {
        return Angle::fromDeg(0.0);
    }

    Angle ans = Angle::fromRad(std::atan(v.y / v.x));

    if (v.x < 0)
        ans += Angle::fromDeg(180);

    return ans;
}

} // namespace Tyr::Common
