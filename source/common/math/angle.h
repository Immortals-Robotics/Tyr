#pragma once

#include "vec2.h"

namespace Tyr::Common
{
struct Angle
{
    static constexpr float kDeg2Rad = std::numbers::pi / 180.0;
    static constexpr float kRad2Deg = 180.0 / std::numbers::pi;

    Angle() = default;

    explicit Angle(const Protos::Immortals::Angle &t_angle) : Angle(fromDeg(t_angle.deg()))
    {}

    void fillProto(Protos::Immortals::Angle *const t_angle) const
    {
        t_angle->set_deg(deg());
    }

    static Angle fromDeg(const float t_deg)
    {
        Angle ang;
        ang.setDeg(t_deg);
        return ang;
    }

    static Angle fromRad(const float t_rad)
    {
        Angle ang;
        ang.setRad(t_rad);
        return ang;
    }

    [[nodiscard]] static Angle fromVec(const Vec2 t_vec)
    {
        if (t_vec.y == 0.0 && t_vec.x == 0.0)
        {
            return fromDeg(0.0);
        }

        Angle ans = fromRad(std::atan(t_vec.y / t_vec.x));

        if (t_vec.x < 0)
            ans += fromDeg(180);

        return ans;
    }

    void setDeg(const float t_deg)
    {
        m_deg = t_deg;
        normalize();
    }

    void setRad(const float t_rad)
    {
        setDeg(t_rad * kRad2Deg);
    }

    [[nodiscard]] float rad() const
    {
        return m_deg * kDeg2Rad;
    }

    [[nodiscard]] float deg() const
    {
        return m_deg;
    }

    [[nodiscard]] float deg360() const
    {
        return std::fmod(m_deg + 360.0f, 360.0f);
    }

    [[nodiscard]] float sin() const
    {
        return std::sin(rad());
    }

    [[nodiscard]] float cos() const
    {
        return std::cos(rad());
    }

    [[nodiscard]] float tan() const
    {
        return std::tan(rad());
    }

    [[nodiscard]] Vec2 toUnitVec() const
    {
        const float rad = this->rad();
        return {std::cos(rad), std::sin(rad)};
    }

    [[nodiscard]] bool isBetween(const Angle t_a, const Angle t_b) const
    {
        // convert all angles to 0..360
        const float ang_360 = deg360();
        const float a_360   = t_a.deg360();
        const float b_360   = t_b.deg360();

        const float ang_min_360 = std::min(a_360, b_360);
        const float ang_max_360 = std::max(a_360, b_360);

        return ang_min_360 < ang_360 && ang_360 < ang_max_360;
    }

    static Angle average(const Angle t_a, const Angle t_b)
    {
        const Vec2 average_vector = (t_a.toUnitVec() + t_b.toUnitVec()) / 2.0f;
        return average_vector.toAngle();
    }

    Angle operator+(const Angle t_ang) const
    {
        return fromDeg(m_deg + t_ang.m_deg);
    }

    void operator+=(const Angle t_ang)
    {
        setDeg(m_deg + t_ang.m_deg);
    }

    Angle operator-(const Angle t_ang) const
    {
        return fromDeg(m_deg - t_ang.m_deg);
    }

    void operator-=(const Angle t_ang)
    {
        setDeg(m_deg - t_ang.m_deg);
    }

    bool operator<(const Angle t_ang) const
    {
        return (t_ang - *this).deg() > 0;
    }

    bool operator>(const Angle t_ang) const
    {
        return (t_ang - *this).deg() < 0;
    }

    Angle operator-() const
    {
        return fromDeg(-m_deg);
    }

    Angle operator*(const float t_f) const
    {
        return fromDeg(m_deg * t_f);
    }

    Angle operator/(const float t_f) const
    {
        return fromDeg(m_deg / t_f);
    }

private:
    // limits the angle to the range of -180 to 180
    void normalize()
    {
        m_deg = std::remainder(m_deg, 360.0f);
    }

    float m_deg = 0.0;
};

// Vec2 methods that depend on Angle

[[nodiscard]] inline Vec2 Vec2::rotated(const Angle t_ang) const
{
    const Angle rotated_angle = toAngle() + t_ang;
    return rotated_angle.toUnitVec() * length();
}

[[nodiscard]] inline Angle Vec2::toAngle() const
{
    return Angle::fromVec(*this);
}

[[nodiscard]] inline Angle Vec2::angleWith(const Vec2 t_v) const
{
    return (t_v - *this).toAngle();
}

[[nodiscard]] inline Angle Vec2::angleDiff(const Vec2 t_v) const
{
    return t_v.toAngle() - this->toAngle();
}

[[nodiscard]] inline Vec2 Vec2::circleAroundPoint(const Angle t_angle, const float t_radius) const
{
    return *this + t_angle.toUnitVec() * t_radius;
}

} // namespace Tyr::Common

template <>
struct fmt::formatter<Tyr::Common::Angle> : fmt::formatter<std::string>
{
    auto format(const Tyr::Common::Angle t_angle, format_context &t_ctx) const
    {
        return fmt::format_to(t_ctx.out(), "{} deg", t_angle.deg());
    }
};
