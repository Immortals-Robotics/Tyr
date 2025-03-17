#pragma once

namespace Immortals::Common
{
struct Angle;

struct Vec3
{
    Vec3() = default;

    explicit constexpr Vec3(const float t_f) : Vec3(t_f, t_f, t_f)
    {}

    constexpr Vec3(const float t_x, const float t_y, const float t_z) : x(t_x), y(t_y), z(t_z)
    {}

    explicit Vec3(const Protos::Immortals::Vec3 &t_v) : Vec3(t_v.x(), t_v.y(), t_v.z())
    {}

    explicit Vec3(const Protos::Ssl::Vector3 &t_v) : Vec3(t_v.x(), t_v.y(), t_v.z())
    {}

#if FEATURE_RAYLIB
    explicit Vec3(const ::Vector3 &t_v) : Vec3(t_v.x, t_v.y, t_v.z)
    {}

    explicit operator ::Vector3() const
    {
        return {.x = x, .y = y, .z = z};
    }
#endif

    void fillProto(Protos::Immortals::Vec3 *const t_v) const
    {
        t_v->set_x(x);
        t_v->set_y(y);
        t_v->set_z(z);
    }

    void fillProto(Protos::Ssl::Vector3 *const t_v) const
    {
        t_v->set_x(x);
        t_v->set_y(y);
        t_v->set_z(z);
    }

    [[nodiscard]] Vec3 normalized() const
    {
        const float length = this->length();

        return almostEqual(length, 0.0f) ? Vec3{} : *this / length;
    }

    [[nodiscard]] float length() const
    {
        return sqrt(x * x + y * y + z * z);
    }

    [[nodiscard]] float lengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] float dot(const Vec3 t_v) const
    {
        return x * t_v.x + y * t_v.y + z * t_v.z;
    }

    [[nodiscard]] float distanceTo(const Vec3 t_v) const
    {
        return (t_v - *this).length();
    }

    [[nodiscard]] float distanceSquaredTo(const Vec3 t_v) const
    {
        return (t_v - *this).lengthSquared();
    }

    [[nodiscard]] Vec3 operator+(const Vec3 t_v) const
    {
        return Vec3{x + t_v.x, y + t_v.y, z + t_v.z};
    }

    [[nodiscard]] Vec3 operator-(const Vec3 t_v) const
    {
        return Vec3{x - t_v.x, y - t_v.y, z - t_v.z};
    }

    [[nodiscard]] Vec3 operator*(const Vec3 t_v) const
    {
        return Vec3{x * t_v.x, y * t_v.y, z * t_v.z};
    }

    [[nodiscard]] Vec3 operator/(const Vec3 t_v) const
    {
        return Vec3{x / t_v.x, y / t_v.y, z / t_v.z};
    }

    [[nodiscard]] Vec3 operator*(const float t_d) const
    {
        return Vec3{x * t_d, y * t_d, z * t_d};
    }

    [[nodiscard]] Vec3 operator/(const float t_d) const
    {
        return Vec3{x / t_d, y / t_d, z / t_d};
    }

    Vec3 &operator+=(const Vec3 t_v)
    {
        x += t_v.x;
        y += t_v.y;
        z += t_v.z;

        return *this;
    }

    Vec3 &operator-=(const Vec3 t_v)
    {
        x -= t_v.x;
        y -= t_v.y;
        z -= t_v.z;

        return *this;
    }

    Vec3 &operator*=(const Vec3 t_v)
    {
        x *= t_v.x;
        y *= t_v.y;
        z *= t_v.z;

        return *this;
    }

    Vec3 &operator/=(const Vec3 t_v)
    {
        x /= t_v.x;
        y /= t_v.y;
        z /= t_v.z;

        return *this;
    }

    Vec3 &operator*=(const float t_d)
    {
        x *= t_d;
        y *= t_d;
        z *= t_d;

        return *this;
    }

    Vec3 &operator/=(const float t_d)
    {
        x /= t_d;
        y /= t_d;
        z /= t_d;

        return *this;
    }

    [[nodiscard]] Vec3 operator-() const
    {
        return Vec3{-x, -y, -z};
    }

    [[nodiscard]] Vec3 operator+() const
    {
        return *this;
    }

    [[nodiscard]] bool operator==(const Vec3 t_v) const
    {
        return almostEqual(x, t_v.x) && almostEqual(y, t_v.y) && almostEqual(z, t_v.z);
    }

    [[nodiscard]] bool operator!=(const Vec3 t_v) const
    {
        return !(*this == t_v);
    }

    [[nodiscard]] Vec2 xy() const
    {
        return {x, y};
    }

    [[nodiscard]] Vec3 abs() const
    {
        return {std::abs(x), std::abs(y), std::abs(z)};
    }

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

} // namespace Immortals::Common

#if FEATURE_LOGGING
template <>
struct fmt::formatter<Immortals::Common::Vec3> : fmt::formatter<std::string>
{
    auto format(Immortals::Common::Vec3 t_v, format_context &t_ctx) const
    {
        return fmt::format_to(t_ctx.out(), "[{}, {}, {}]", t_v.x, t_v.y, t_v.z);
    }
};
#endif
