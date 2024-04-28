#include "vector.h"

#include "angle.h"
#include "helpers.h"

namespace Tyr::Common
{
Vec2::Vec2(const float t_x, const float t_y) : x(t_x), y(t_y)
{}

Vec3::Vec3(const float t_x, const float t_y, const float t_z) : x(t_x), y(t_y), z(t_z)
{}

Vec2 Vec2::operator+(const Vec2 t_v) const
{
    return Vec2{x + t_v.x, y + t_v.y};
}

Vec2 Vec2::operator-(const Vec2 t_v) const
{
    return Vec2{x - t_v.x, y - t_v.y};
}

Vec2 Vec2::operator*(const Vec2 t_v) const
{
    return Vec2{x * t_v.x, y * t_v.y};
}

Vec2 Vec2::operator/(const Vec2 t_v) const
{
    return Vec2{x / t_v.x, y / t_v.y};
}

Vec2 Vec2::operator*(const float t_d) const
{
    return Vec2{x * t_d, y * t_d};
}

Vec2 Vec2::operator/(const float t_d) const
{
    return Vec2{x / t_d, y / t_d};
}

Vec2 &Vec2::operator+=(const Vec2 t_v)
{
    x += t_v.x;
    y += t_v.y;

    return *this;
}

Vec2 &Vec2::operator-=(const Vec2 t_v)
{
    x -= t_v.x;
    y -= t_v.y;

    return *this;
}

Vec2 &Vec2::operator*=(const Vec2 t_v)
{
    x *= t_v.x;
    y *= t_v.y;

    return *this;
}

Vec2 &Vec2::operator/=(const Vec2 t_v)
{
    x /= t_v.x;
    y /= t_v.y;

    return *this;
}

Vec2 &Vec2::operator*=(const float t_d)
{
    x *= t_d;
    y *= t_d;

    return *this;
}

Vec2 &Vec2::operator/=(const float t_d)
{
    x /= t_d;
    y /= t_d;

    return *this;
}

Vec2 Vec2::operator-() const
{
    return Vec2{-x, -y};
}

Vec2 Vec2::operator+() const
{
    return *this;
}

Vec3 Vec3::operator+(const Vec3 t_v) const
{
    return Vec3{x + t_v.x, y + t_v.y, z + t_v.z};
}

Vec3 Vec3::operator-(const Vec3 t_v) const
{
    return Vec3{x - t_v.x, y - t_v.y, z - t_v.z};
}

Vec3 Vec3::operator*(const Vec3 t_v) const
{
    return Vec3{x * t_v.x, y * t_v.y, z * t_v.z};
}

Vec3 Vec3::operator/(const Vec3 t_v) const
{
    return Vec3{x / t_v.x, y / t_v.y, z / t_v.z};
}

Vec3 Vec3::operator*(const float t_d) const
{
    return Vec3{x * t_d, y * t_d, z * t_d};
}

Vec3 Vec3::operator/(const float t_d) const
{
    return Vec3{x / t_d, y / t_d, z / t_d};
}

Vec3 &Vec3::operator+=(const Vec3 t_v)
{
    x += t_v.x;
    y += t_v.y;
    z += t_v.z;

    return *this;
}

Vec3 &Vec3::operator-=(const Vec3 t_v)
{
    x -= t_v.x;
    y -= t_v.y;
    z -= t_v.z;

    return *this;
}

Vec3 &Vec3::operator*=(const Vec3 t_v)
{
    x *= t_v.x;
    y *= t_v.y;
    z *= t_v.z;

    return *this;
}

Vec3 &Vec3::operator/=(const Vec3 t_v)
{
    x /= t_v.x;
    y /= t_v.y;
    z /= t_v.z;

    return *this;
}

Vec3 &Vec3::operator*=(const float t_d)
{
    x *= t_d;
    y *= t_d;
    z *= t_d;

    return *this;
}

Vec3 &Vec3::operator/=(const float t_d)
{
    x /= t_d;
    y /= t_d;
    z /= t_d;

    return *this;
}

Vec3 Vec3::operator-() const
{
    return Vec3{-x, -y, -z};
}

Vec3 Vec3::operator+() const
{
    return *this;
}

Vec2 Vec2::normalized() const
{
    const float magnitude = this->magnitude();

    return magnitude == 0.0 ? Vec2{0.0, 0.0} : *this / magnitude;
}

Vec3 Vec3::normalized() const
{
    const float magnitude = this->magnitude();

    return magnitude == 0.0 ? Vec3{0.0, 0.0, 0.0} : *this / magnitude;
}

float Vec2::magnitude() const
{
    return sqrt(x * x + y * y);
}

float Vec2::magnitudePow2() const
{
    return x * x + y * y;
}

float Vec3::magnitude() const
{
    return sqrt(x * x + y * y + z * z);
}

float Vec2::dot(const Vec2 t_v)
{
    return x * t_v.x + y * t_v.y;
}

float Vec2::distanceTo(Vec2 t_v)
{
    return (t_v - *this).magnitude();
}

Angle Vec2::angleWith(const Vec2 t_v)
{
    return (t_v - *this).toAngle();
}

Angle Vec2::angleDiff(const Vec2 t_v)
{
    return t_v.toAngle() - this->toAngle();
}

Vec2 Vec2::circleAroundPoint(Angle angle, float radius)
{
    return *this + angle.toUnitVec() * radius;
}

Vec2 Vec2::pointOnConnectingLine(Vec2 first_point, Vec2 second_point, float distance)
{
    float m = (second_point.y - first_point.y) / (second_point.x - first_point.x);
    Vec2  ans;
    if (second_point.x - first_point.x > 0)
        ans.x = first_point.x + distance / std::sqrt(std::pow(m, 2.0f) + 1.0f);
    else
        ans.x = first_point.x - distance / std::sqrt(std::pow(m, 2.0f) + 1.0f);
    ans.y = first_point.y + m * (ans.x - first_point.x);
    return ans;
}

float Vec3::dot(const Vec3 t_a, const Vec3 t_b)
{
    return t_a.x * t_b.x + t_a.y * t_b.y + t_a.z * t_b.z;
}

Vec2 Vec2::rotated(const Angle t_ang) const
{
    const Angle rotated_angle = toAngle() + t_ang;
    return rotated_angle.toUnitVec() * magnitude();
}

bool Vec2::operator==(const Vec2 t_v) const
{
    return almostEqual(x, t_v.x) && almostEqual(y, t_v.y);
}

bool Vec2::operator!=(const Vec2 t_v) const
{
    return !(*this == t_v);
}

Angle Vec2::toAngle() const
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

} // namespace Tyr::Common
