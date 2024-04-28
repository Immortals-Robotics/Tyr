#pragma once

namespace Tyr::Common
{
struct Angle;

struct Vec2
{
    float x = 0.0;
    float y = 0.0;

    Vec2() = default;
    Vec2(float t_x, float t_y);

    [[nodiscard]] Vec2  normalized() const;
    [[nodiscard]] float magnitude() const;
    [[nodiscard]] float magnitude_pow2() const;
    [[nodiscard]] Vec2  rotated(Angle t_ang) const;

    [[nodiscard]] Angle toAngle() const;

    static float dot(Vec2 t_a, Vec2 t_b);

    static Angle angle_with(Vec2 a, Vec2 b);
    static Angle angle_diff(Vec2 a, Vec2 b);

    Vec2 point_on_connecting_line(Vec2 FirstPoint, Vec2 SecondPoint, float distance);
    Vec2 circle_around_point(Vec2 point, Angle angle, float radius);

    Vec2 operator+(Vec2 t_v) const;
    Vec2 operator-(Vec2 t_v) const;
    Vec2 operator*(Vec2 t_v) const;
    Vec2 operator/(Vec2 t_v) const;

    Vec2 operator*(float t_d) const;
    Vec2 operator/(float t_d) const;

    Vec2 &operator+=(Vec2 t_v);
    Vec2 &operator-=(Vec2 t_v);
    Vec2 &operator*=(Vec2 t_v);
    Vec2 &operator/=(Vec2 t_v);

    Vec2 &operator*=(float t_d);
    Vec2 &operator/=(float t_d);

    Vec2 operator-() const;
    Vec2 operator+() const;

    /**
     * This operation calculates the distance of two vectors
     */
    float operator^(Vec2 t_v) const;

    // TODO: floating-point numbers cannot be checked for equality
    // either drop these or implement an approximation
    bool operator==(Vec2 t_v) const;
    bool operator!=(Vec2 t_v) const;

    friend std::ostream &operator<<(std::ostream &t_oo, const Vec2 &t_vec)
    {
        t_oo << "X: " << t_vec.x << ", Y: " << t_vec.y << std::endl;
        return t_oo;
    }
};

struct Vec3
{
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

	Vec3() = default;
    Vec3(float t_x, float t_y, float t_z);

    [[nodiscard]] Vec3  normalized() const;
    [[nodiscard]] float magnitude() const;

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
