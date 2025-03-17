#pragma once

#include "helpers.h"

namespace Immortals::Common
{
struct Angle;

struct Vec2
{
    Vec2() = default;

    explicit constexpr Vec2(const float t_f) : Vec2(t_f, t_f)
    {}

    constexpr Vec2(const float t_x, const float t_y) : x(t_x), y(t_y)
    {}

    explicit Vec2(const Protos::Immortals::Vec2 &t_v) : Vec2(t_v.x(), t_v.y())
    {}

    explicit Vec2(const Protos::Ssl::Vector2 &t_v) : Vec2(t_v.x(), t_v.y())
    {}

#if FEATURE_RAYLIB
    explicit Vec2(const ::Vector2 &t_v) : Vec2(t_v.x, t_v.y)
    {}

    explicit operator ::Vector2() const
    {
        return {.x = x, .y = y};
    }
#endif

#if FEATURE_IMGUI
    explicit Vec2(const ::ImVec2 &t_v) : Vec2(t_v.x, t_v.y)
    {}

    explicit operator ::ImVec2() const
    {
        return {x, y};
    }
#endif

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

        return almostEqual(length, 0.0f) ? Vec2{} : *this / length;
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

    // calculates the cross product of (x,y,0) and (t_v.x,t_v.y,0)
    // the resulting vector is perpendicular to xy-plane and therefore
    // only has a z component, which is the return value of this function.
    [[nodiscard]] float cross(const Vec2 t_v) const
    {
        return x * t_v.y - y * t_v.x;
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

    // Component-wise max operation
    static Vec2 max(const Vec2& a, const Vec2& b) {
        return {std::max(a.x, b.x), std::max(a.y, b.y)};
    }

    // Component-wise min operation
    static Vec2 min(const Vec2& a, const Vec2& b) {
        return {std::min(a.x, b.x), std::min(a.y, b.y)};
    }

    // Component-wise clamp
    static Vec2 clamp(const Vec2& value, const Vec2& minVal, const Vec2& maxVal) {
        return {
            std::clamp(value.x, minVal.x, maxVal.x),
            std::clamp(value.y, minVal.y, maxVal.y)};
    }

    float x = 0.0f;
    float y = 0.0f;
};
} // namespace Immortals::Common

#if FEATURE_LOGGING
template <>
struct fmt::formatter<Immortals::Common::Vec2> : fmt::formatter<std::string>
{
    auto format(Immortals::Common::Vec2 t_v, format_context &t_ctx) const
    {
        return fmt::format_to(t_ctx.out(), "[{}, {}]", t_v.x, t_v.y);
    }
};
#endif
