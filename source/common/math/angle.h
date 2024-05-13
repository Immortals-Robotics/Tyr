#pragma once

namespace Tyr::Common
{
struct Vec2;

struct Angle
{
    static constexpr float kDeg2Rad = std::numbers::pi / 180.0;
    static constexpr float kRad2Deg = 180.0 / std::numbers::pi;

    Angle() = default;

    Angle(const Protos::Immortals::Angle &t_angle) : Angle(fromDeg(t_angle.deg()))
    {}

    inline void fillProto(Protos::Immortals::Angle *const t_angle) const
    {
        t_angle->set_deg(deg());
    }

    inline static Angle fromDeg(float deg)
    {
        Angle ang;
        ang.setDeg(deg);
        return ang;
    }

    inline static Angle fromRad(float rad)
    {
        Angle ang;
        ang.setRad(rad);
        return ang;
    }

    inline void setDeg(float deg)
    {
        m_deg = deg;
        normalize();
    }

    inline void setRad(float rad)
    {
        setDeg(rad * kRad2Deg);
    }

    [[nodiscard]] inline float rad() const
    {
        return m_deg * kDeg2Rad;
    }

    [[nodiscard]] inline float deg() const
    {
        return m_deg;
    }

    [[nodiscard]] inline float deg360() const
    {
        return std::fmod(m_deg + 360.0f, 360.0f);
    }

    [[nodiscard]] inline float sin() const
    {
        return std::sin(rad());
    }

    [[nodiscard]] inline float cos() const
    {
        return std::cos(rad());
    }

    [[nodiscard]] inline float tan() const
    {
        return std::tan(rad());
    }

    [[nodiscard]] Vec2 toUnitVec() const;

    [[nodiscard]] inline bool isBetween(Angle a, Angle b) const
    {
        // convert all angles to 0..360
        const float ang_360 = deg360();
        const float a_360   = a.deg360();
        const float b_360   = b.deg360();

        const float ang_min_360 = std::min(a_360, b_360);
        const float ang_max_360 = std::max(a_360, b_360);

        return ang_min_360 < ang_360 && ang_360 < ang_max_360;
    }

    static Angle average(Angle a, Angle b);

    inline Angle operator+(Angle ang) const
    {
        return fromDeg(m_deg + ang.m_deg);
    }

    inline void operator+=(Angle ang)
    {
        setDeg(m_deg + ang.m_deg);
    }

    inline Angle operator-(Angle ang) const
    {
        return fromDeg(m_deg - ang.m_deg);
    }

    inline void operator-=(Angle ang)
    {
        setDeg(m_deg - ang.m_deg);
    }

    inline bool operator<(Angle ang) const
    {
        return (ang - *this).deg() > 0;
    }

    inline bool operator>(Angle ang) const
    {
        return (ang - *this).deg() < 0;
    }

    inline Angle operator-() const
    {
        return fromDeg(-m_deg);
    }

    inline Angle operator*(float f) const
    {
        return fromDeg(m_deg * f);
    }

    inline Angle operator/(float f) const
    {
        return fromDeg(m_deg / f);
    }

private:
    // limits the angle to the range of -180 to 180
    inline void normalize()
    {
        m_deg = std::remainder(m_deg, 360.0f);
    }

    float m_deg = 0.0;
};

} // namespace Tyr::Common

template <>
struct fmt::formatter<Tyr::Common::Angle> : fmt::formatter<std::string>
{
    auto format(Tyr::Common::Angle t_angle, format_context &t_ctx) const
    {
        return fmt::format_to(t_ctx.out(), "{} deg", t_angle.deg());
    }
};
