#pragma once

#include "helpers.h"

namespace Tyr::Common
{
struct Angle;

struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    constexpr Vec2(float t_f) : Vec2(t_f, t_f)
    {}
    constexpr Vec2(float t_x, float t_y) : x(t_x), y(t_y)
    {}

    Vec2(const Protos::Immortals::Vec2 &t_v) : Vec2(t_v.x(), t_v.y())
    {}

    void fillProto(Protos::Immortals::Vec2 *const t_v) const
    {
        t_v->set_x(x);
        t_v->set_y(y);
    }

    [[nodiscard]] inline Vec2 normalized() const
    {
        const float length = this->length();

        return length == 0.0 ? Vec2{0.0, 0.0} : *this / length;
    }

    [[nodiscard]] inline float length() const
    {
        return sqrt(x * x + y * y);
    }
    [[nodiscard]] inline float lengthSquared() const
    {
        return x * x + y * y;
    }

    [[nodiscard]] inline Vec2 rotated(Angle t_ang) const
    {
        const Angle rotated_angle = toAngle() + t_ang;
        return rotated_angle.toUnitVec() * length();
    }

    [[nodiscard]] inline Angle toAngle() const
    {
        if (y == 0.0 && x == 0.0)
        {
            return Angle::fromDeg(0.0);
        }

        Angle ans = Angle::fromRad(std::atan(y / x));

        if (x < 0)
            ans += Angle::fromDeg(180);

        return ans;
    }

    [[nodiscard]] inline float dot(Vec2 t_v) const
    {
        return x * t_v.x + y * t_v.y;
    }

    [[nodiscard]] inline float distanceTo(Vec2 t_v) const
    {
        return (t_v - *this).length();
    }

    [[nodiscard]] inline float distanceSquaredTo(Vec2 t_v) const
    {
        return (t_v - *this).lengthSquared();
    }

    [[nodiscard]] inline Angle angleWith(Vec2 t_v) const
    {
        return (t_v - *this).toAngle();
    }

    [[nodiscard]] inline Angle angleDiff(Vec2 t_v) const
    {
        return t_v.toAngle() - this->toAngle();
    }

    // TODO: this should be replaced by line / circle usage
    [[nodiscard]] inline Vec2 pointOnConnectingLine(Vec2 secondPoint, float distance) const
    {
        float m = (secondPoint.y - y) / (secondPoint.x - x);
        Vec2  ans;
        if (secondPoint.x - x > 0)
            ans.x = x + distance / std::sqrt(std::pow(m, 2.0f) + 1.0f);
        else
            ans.x = x - distance / std::sqrt(std::pow(m, 2.0f) + 1.0f);
        ans.y = y + m * (ans.x - x);
        return ans;
    }

    [[nodiscard]] inline Vec2 circleAroundPoint(Angle angle, float radius) const
    {
        return *this + angle.toUnitVec() * radius;
    }

    [[nodiscard]] inline Vec2 operator+(Vec2 t_v) const
    {
        return Vec2{x + t_v.x, y + t_v.y};
    }

    [[nodiscard]] inline Vec2 operator-(Vec2 t_v) const
    {
        return Vec2{x - t_v.x, y - t_v.y};
    }

    [[nodiscard]] inline Vec2 operator*(Vec2 t_v) const
    {
        return Vec2{x * t_v.x, y * t_v.y};
    }

    [[nodiscard]] inline Vec2 operator/(Vec2 t_v) const
    {
        return Vec2{x / t_v.x, y / t_v.y};
    }

    [[nodiscard]] inline Vec2 operator*(float t_d) const
    {
        return Vec2{x * t_d, y * t_d};
    }

    [[nodiscard]] inline Vec2 operator/(float t_d) const
    {
        return Vec2{x / t_d, y / t_d};
    }

    inline Vec2 &operator+=(Vec2 t_v)
    {
        x += t_v.x;
        y += t_v.y;

        return *this;
    }

    inline Vec2 &operator-=(Vec2 t_v)
    {
        x -= t_v.x;
        y -= t_v.y;

        return *this;
    }

    inline Vec2 &operator*=(Vec2 t_v)
    {
        x *= t_v.x;
        y *= t_v.y;

        return *this;
    }

    inline Vec2 &operator/=(Vec2 t_v)
    {
        x /= t_v.x;
        y /= t_v.y;

        return *this;
    }

    inline Vec2 &operator*=(float t_d)
    {
        x *= t_d;
        y *= t_d;

        return *this;
    }

    inline Vec2 &operator/=(float t_d)
    {
        x /= t_d;
        y /= t_d;

        return *this;
    }

    [[nodiscard]] inline Vec2 operator-() const
    {
        return Vec2{-x, -y};
    }

    [[nodiscard]] inline Vec2 operator+() const
    {
        return *this;
    }

    [[nodiscard]] inline bool operator==(Vec2 t_v) const
    {
        return almostEqual(x, t_v.x) && almostEqual(y, t_v.y);
    }

    [[nodiscard]] inline bool operator!=(Vec2 t_v) const
    {
        return !(*this == t_v);
    }
};

struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    constexpr Vec3(float t_f) : Vec3(t_f, t_f, t_f)
    {}
    constexpr Vec3(float t_x, float t_y, float t_z) : x(t_x), y(t_y), z(t_z)
    {}

    Vec3(const Protos::Immortals::Vec3 &t_v) : Vec3(t_v.x(), t_v.y(), t_v.z())
    {}

    void fillProto(Protos::Immortals::Vec3 *const t_v) const
    {
        t_v->set_x(x);
        t_v->set_y(y);
        t_v->set_z(z);
    }

    [[nodiscard]] Vec3  normalized() const;
    [[nodiscard]] float length() const;

    [[nodiscard]] float dot(Vec3 t_v);

    [[nodiscard]] Vec3 operator+(Vec3 t_v) const;
    [[nodiscard]] Vec3 operator-(Vec3 t_v) const;
    [[nodiscard]] Vec3 operator*(Vec3 t_v) const;
    [[nodiscard]] Vec3 operator/(Vec3 t_v) const;

    [[nodiscard]] Vec3 operator*(float t_d) const;
    [[nodiscard]] Vec3 operator/(float t_d) const;

    Vec3 &operator+=(Vec3 t_v);
    Vec3 &operator-=(Vec3 t_v);
    Vec3 &operator*=(Vec3 t_v);
    Vec3 &operator/=(Vec3 t_v);

    Vec3 &operator*=(float t_d);
    Vec3 &operator/=(float t_d);

    [[nodiscard]] Vec3 operator-() const;
    [[nodiscard]] Vec3 operator+() const;
};

} // namespace Tyr::Common

template <>
struct fmt::formatter<Tyr::Common::Vec2> : fmt::formatter<std::string>
{
    auto format(Tyr::Common::Vec2 t_v, format_context &t_ctx) const
    {
        return fmt::format_to(t_ctx.out(), "[{}, {}]", t_v.x, t_v.y);
    }
};

template <>
struct fmt::formatter<Tyr::Common::Vec3> : fmt::formatter<std::string>
{
    auto format(Tyr::Common::Vec3 t_v, format_context &t_ctx) const
    {
        return fmt::format_to(t_ctx.out(), "[{}, {}, {}]", t_v.x, t_v.y, t_v.z);
    }
};
