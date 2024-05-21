#pragma once

namespace Tyr::Common
{
struct Circle
{
public:
    Circle(const Vec2 t_center, const float t_r) : center(t_center), r(t_r)
    {}

    Circle(const Protos::Immortals::Circle &t_circle) : Circle(t_circle.center(), t_circle.r())
    {}

    inline void fillProto(Protos::Immortals::Circle *const t_circle) const
    {
        center.fillProto(t_circle->mutable_center());
        t_circle->set_r(r);
    }

    inline float circumference() const
    {
        return 2.0 * std::numbers::pi * r;
    }

    inline float area() const
    {
        return std::numbers::pi * r * r;
    }

    inline bool inside(const Vec2 t_point) const
    {
        return (center - t_point).length() < r;
    }

    inline bool insideOffset(const Vec2 t_point, float offset) const
    {
        return (center - t_point).length() < (r + offset);
    }

    inline Vec2 nearestOutside(Vec2 t_point, float t_extra_r = 0.0) const
    {
        Vec2 tmp_vec = t_point - center;
        tmp_vec      = tmp_vec.normalized() * (r + t_extra_r);
        return center + tmp_vec;
    }

    // circle intersection
    std::vector<Vec2> intersect(const Circle &t_other) const;
    float             intersectionArea(const Circle &t_other) const;

    bool isCircleCross(Vec2 t_point1, Vec2 t_point2) const;

    Vec2  center;
    float r;
};
} // namespace Tyr::Common
