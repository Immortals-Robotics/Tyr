#include "vector.h"

namespace Tyr::Common
{
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

Vec3 Vec3::normalized() const
{
    const float length = this->length();

    return length == 0.0 ? Vec3{0.0, 0.0, 0.0} : *this / length;
}


float Vec3::length() const
{
    return sqrt(x * x + y * y + z * z);
}


float Vec3::dot(const Vec3 t_v)
{
    return x * t_v.x + y * t_v.y + z * t_v.z;
}
} // namespace Tyr::Common
