#pragma once

#include "../vector.h"

namespace Tyr::Common
{
struct Triangle
{
    Triangle(const std::array<Vec2, 3> &t_corner) : corner{t_corner}
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