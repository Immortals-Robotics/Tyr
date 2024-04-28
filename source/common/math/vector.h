#pragma once

#include <ostream>

namespace Tyr::Common
{
struct Angle;

struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
	Vec2(float t_f);
    Vec2(float t_x, float t_y);

    [[nodiscard]] Vec2  normalized() const;
    [[nodiscard]] float length() const;
    [[nodiscard]] float lengthPow2() const;
    [[nodiscard]] Vec2  rotated(Angle t_ang) const;

    [[nodiscard]] Angle toAngle() const;

    [[nodiscard]] float dot(Vec2 t_v) const;
    [[nodiscard]] float distanceTo(Vec2 t_v) const;

    [[nodiscard]] Angle angleWith(Vec2 t_v) const;
    [[nodiscard]] Angle angleDiff(Vec2 t_v) const;

	// TODO: this should be replaced by line / circle usage
    [[nodiscard]] Vec2 pointOnConnectingLine(Vec2 secondPoint, float distance) const;
    [[nodiscard]] Vec2 circleAroundPoint(Angle angle, float radius) const;

    [[nodiscard]] Vec2 operator+(Vec2 t_v) const;
    [[nodiscard]] Vec2 operator-(Vec2 t_v) const;
    [[nodiscard]] Vec2 operator*(Vec2 t_v) const;
    [[nodiscard]] Vec2 operator/(Vec2 t_v) const;

    [[nodiscard]] Vec2 operator*(float t_d) const;
    [[nodiscard]] Vec2 operator/(float t_d) const;

    Vec2 &operator+=(Vec2 t_v);
    Vec2 &operator-=(Vec2 t_v);
    Vec2 &operator*=(Vec2 t_v);
    Vec2 &operator/=(Vec2 t_v);

    Vec2 &operator*=(float t_d);
    Vec2 &operator/=(float t_d);

    [[nodiscard]] Vec2 operator-() const;
    [[nodiscard]] Vec2 operator+() const;

    // TODO: floating-point numbers cannot be checked for equality
    // either drop these or implement an approximation
    [[nodiscard]] bool operator==(Vec2 t_v) const;
    [[nodiscard]] bool operator!=(Vec2 t_v) const;

    friend std::ostream &operator<<(std::ostream &t_oo, const Vec2 &t_vec)
    {
        t_oo << "X: " << t_vec.x << ", Y: " << t_vec.y << std::endl;
        return t_oo;
    }
};

struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
	Vec3(float t_f);
    Vec3(float t_x, float t_y, float t_z);

    [[nodiscard]] Vec3  normalized() const;
    [[nodiscard]] float length() const;

    static float dot(Vec3 t_a, Vec3 t_b);

    Vec3 operator+(Vec3 t_v) const;
    Vec3 operator-(Vec3 t_v) const;
    Vec3 operator*(Vec3 t_v) const;
    Vec3 operator/(Vec3 t_v) const;

    Vec3 operator*(float t_d) const;
    Vec3 operator/(float t_d) const;

    Vec3 &operator+=(Vec3 t_v);
    Vec3 &operator-=(Vec3 t_v);
    Vec3 &operator*=(Vec3 t_v);
    Vec3 &operator/=(Vec3 t_v);

    Vec3 &operator*=(float t_d);
    Vec3 &operator/=(float t_d);

    Vec3 operator-() const;
    Vec3 operator+() const;
};

} // namespace Tyr::Common
