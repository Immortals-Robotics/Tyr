#pragma once

#include "../vector.h"

namespace Tyr::Common
{
struct Triangle
{
    Triangle(const Vec2 t_corner_1, const Vec2 t_corner_2, const Vec2 t_corner_3)
        : corner({t_corner_1, t_corner_2, t_corner_3})
    {
        // sort corners clockwise
        const float area = (corner[1].x - corner[0].x) * (corner[2].y - corner[0].y) -
                           (corner[2].x - corner[0].x) * (corner[1].y - corner[0].y);
        if (area < 0)
        {
            std::swap(corner[1], corner[2]);
        }
    }

    Triangle(const std::array<Vec2, 3> &t_corner) : Triangle(t_corner[0], t_corner[1], t_corner[2])
    {}

    Triangle(const Protos::Immortals::Triangle &t_triangle)
        : Triangle({t_triangle.corner(0), t_triangle.corner(1), t_triangle.corner(2)})
    {}

    void fillProto(Protos::Immortals::Triangle *const t_triangle) const
    {
        t_triangle->clear_corner();
        corner[0].fillProto(t_triangle->add_corner());
        corner[1].fillProto(t_triangle->add_corner());
        corner[2].fillProto(t_triangle->add_corner());
    }

    std::array<Vec2, 3> corner;
};
} // namespace Tyr::Common