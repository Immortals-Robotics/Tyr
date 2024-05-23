#pragma once

namespace Tyr::Common
{
struct Circle
{
    Circle(const Vec2 t_center, const float t_r) : center(t_center), r(t_r)
    {}

    explicit Circle(const Protos::Immortals::Circle &t_circle) : Circle(Vec2{t_circle.center()}, t_circle.r())
    {}

    void fillProto(Protos::Immortals::Circle *const t_circle) const
    {
        center.fillProto(t_circle->mutable_center());
        t_circle->set_r(r);
    }

    float circumference() const
    {
        return 2.0f * std::numbers::pi * r;
    }

    float area() const
    {
        return std::numbers::pi * r * r;
    }

    bool inside(const Vec2 t_point) const
    {
        return (center - t_point).length() < r;
    }

    bool insideOffset(const Vec2 t_point, const float t_offset) const
    {
        return (center - t_point).length() < (r + t_offset);
    }

    Vec2 nearestOutside(const Vec2 t_point, const float t_extra_r = 0.0) const
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
