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

    Vec2(const Protos::Ssl::Vector2 &t_v) : Vec2(t_v.x(), t_v.y())
    {}

    void fillProto(Protos::Immortals::Vec2 *const t_v) const
    {
        t_v->set_x(x);
        t_v->set_y(y);
    }

    void fillProto(Protos::Ssl::Vector2 *const t_v) const
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

    [[nodiscard]] inline Vec2 rotated(Angle t_ang) const;

    [[nodiscard]] inline Angle toAngle() const;

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

    [[nodiscard]] inline Angle angleWith(Vec2 t_v) const;

    [[nodiscard]] inline Angle angleDiff(Vec2 t_v) const;

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

    [[nodiscard]] inline Vec2 circleAroundPoint(Angle angle, float radius) const;

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
} // namespace Tyr::Common

template <>
struct fmt::formatter<Tyr::Common::Vec2> : fmt::formatter<std::string>
{
    auto format(Tyr::Common::Vec2 t_v, format_context &t_ctx) const
    {
        return fmt::format_to(t_ctx.out(), "[{}, {}]", t_v.x, t_v.y);
    }
};
