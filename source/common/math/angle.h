#pragma once

#include <numbers>

#include "vector.h"

namespace Tyr::Common
{
struct Angle
{
    Angle() = default;

    static Angle fromDeg(float deg);
    static Angle fromRad(float rad);

    void setDeg(float deg);
    void setRad(float rad);

    [[nodiscard]] float rad() const;
    [[nodiscard]] float deg() const;

    [[nodiscard]] float sin() const;
    [[nodiscard]] float cos() const;
    [[nodiscard]] float tan() const;

    [[nodiscard]] vec2 toUnitVec() const;

    [[nodiscard]] bool isBetween(Angle a, Angle b) const;

    static Angle average(Angle a, Angle b);

    Angle operator+(Angle ang) const;
    void  operator+=(Angle ang);
    Angle operator-(Angle ang) const;
    void  operator-=(Angle ang);
    bool  operator<(Angle ang) const;
    bool  operator>(Angle ang) const;

    Angle operator*(float f) const;
    Angle operator/(float f) const;

private:
    // limits the angle to the range of -180 to 180
    void normalize();

    float m_deg = 0.0;
};

Angle angle_with(const vec2 &a, const vec2 &b);
Angle to_angle(const vec2 &v);

} // namespace Tyr::Common
