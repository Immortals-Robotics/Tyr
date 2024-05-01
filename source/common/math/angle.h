#pragma once

namespace Tyr::Common
{
struct Vec2;

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

    [[nodiscard]] Vec2 toUnitVec() const;

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

} // namespace Tyr::Common

template <>
struct fmt::formatter<Tyr::Common::Angle> : fmt::formatter<std::string>
{
    auto format(Tyr::Common::Angle t_angle, format_context &t_ctx) const
    {
        return fmt::format_to(t_ctx.out(), "{} deg", t_angle.deg());
    }
};
