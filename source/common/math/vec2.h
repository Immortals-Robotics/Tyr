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

    explicit constexpr Vec2(const float t_f) : Vec2(t_f, t_f)
    {}

    constexpr Vec2(const float t_x, const float t_y) : x(t_x), y(t_y)
    {}

    explicit Vec2(const Protos::Immortals::Vec2 &t_v) : Vec2(t_v.x(), t_v.y())
    {}

    explicit Vec2(const Protos::Ssl::Vector2 &t_v) : Vec2(t_v.x(), t_v.y())
    {}

    explicit Vec2(const ::Vector2 &t_v) : Vec2(t_v.x, t_v.y)
    {}

    explicit Vec2(const ::ImVec2 &t_v) : Vec2(t_v.x, t_v.y)
    {}

    explicit operator ::Vector2() const
    {
        return {.x = x, .y = y};
    }

    explicit operator ::ImVec2() const
    {
        return {x, y};
    }

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

    [[nodiscard]] Vec2 normalized() const
    {
        const float length = this->length();

        return length == 0.0 ? Vec2{0.0, 0.0} : *this / length;
    }

    [[nodiscard]] float length() const
    {
        return sqrt(x * x + y * y);
    }
    [[nodiscard]] float lengthSquared() const
    {
        return x * x + y * y;
    }

    [[nodiscard]] inline Vec2 rotated(Angle t_ang) const;

    [[nodiscard]] inline Angle toAngle() const;

    [[nodiscard]] float dot(const Vec2 t_v) const
    {
        return x * t_v.x + y * t_v.y;
    }

    [[nodiscard]] float distanceTo(const Vec2 t_v) const
    {
        return (t_v - *this).length();
    }

    [[nodiscard]] float distanceSquaredTo(const Vec2 t_v) const
    {
        return (t_v - *this).lengthSquared();
    }

    [[nodiscard]] inline Angle angleWith(Vec2 t_v) const;

    [[nodiscard]] inline Angle angleDiff(Vec2 t_v) const;

    // TODO: this should be replaced by line / circle usage
    [[nodiscard]] Vec2 pointOnConnectingLine(const Vec2 t_second_point, const float t_distance) const
    {
        const float m = (t_second_point.y - y) / (t_second_point.x - x);

        Vec2 ans;
        if (t_second_point.x - x > 0)
            ans.x = x + t_distance / std::sqrt(std::pow(m, 2.0f) + 1.0f);
        else
            ans.x = x - t_distance / std::sqrt(std::pow(m, 2.0f) + 1.0f);
        ans.y = y + m * (ans.x - x);
        return ans;
    }

    [[nodiscard]] inline Vec2 circleAroundPoint(Angle t_angle, float t_radius) const;

    [[nodiscard]] Vec2 operator+(const Vec2 t_v) const
    {
        return Vec2{x + t_v.x, y + t_v.y};
    }

    [[nodiscard]] Vec2 operator-(const Vec2 t_v) const
    {
        return Vec2{x - t_v.x, y - t_v.y};
    }

    [[nodiscard]] Vec2 operator*(const Vec2 t_v) const
    {
        return Vec2{x * t_v.x, y * t_v.y};
    }

    [[nodiscard]] Vec2 operator/(const Vec2 t_v) const
    {
        return Vec2{x / t_v.x, y / t_v.y};
    }

    [[nodiscard]] Vec2 operator*(const float t_d) const
    {
        return Vec2{x * t_d, y * t_d};
    }

    [[nodiscard]] Vec2 operator/(const float t_d) const
    {
        return Vec2{x / t_d, y / t_d};
    }

    Vec2 &operator+=(const Vec2 t_v)
    {
        x += t_v.x;
        y += t_v.y;

        return *this;
    }

    Vec2 &operator-=(const Vec2 t_v)
    {
        x -= t_v.x;
        y -= t_v.y;

        return *this;
    }

    Vec2 &operator*=(const Vec2 t_v)
    {
        x *= t_v.x;
        y *= t_v.y;

        return *this;
    }

    Vec2 &operator/=(const Vec2 t_v)
    {
        x /= t_v.x;
        y /= t_v.y;

        return *this;
    }

    Vec2 &operator*=(const float t_d)
    {
        x *= t_d;
        y *= t_d;

        return *this;
    }

    Vec2 &operator/=(const float t_d)
    {
        x /= t_d;
        y /= t_d;

        return *this;
    }

    [[nodiscard]] Vec2 operator-() const
    {
        return Vec2{-x, -y};
    }

    [[nodiscard]] Vec2 operator+() const
    {
        return *this;
    }

    [[nodiscard]] bool operator==(const Vec2 t_v) const
    {
        return almostEqual(x, t_v.x) && almostEqual(y, t_v.y);
    }

    [[nodiscard]] bool operator!=(const Vec2 t_v) const
    {
        return !(*this == t_v);
    }

    [[nodiscard]] Vec2 xx() const
    {
        return {x, x};
    }

    [[nodiscard]] Vec2 yy() const
    {
        return {y, y};
    }

    [[nodiscard]] Vec2 yx() const
    {
        return {y, x};
    }

    [[nodiscard]] Vec2 abs() const
    {
        return {std::abs(x), std::abs(y)};
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
