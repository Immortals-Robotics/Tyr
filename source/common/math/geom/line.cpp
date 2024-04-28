#include "line.h"

#include "../helpers.h"
#include "line_segment.h"

namespace Tyr::Common
{
Line::Line(const float t_a, const float t_b, const float t_c) : a(t_a), b(t_b), c(t_c)
{}

Line Line::fromTwoPoints(const Vec2 t_pos_a, const Vec2 t_pos_b)
{
    const Vec2 d_pos = t_pos_b - t_pos_a;

    float a;
    float b;

    if (almostEqual(d_pos.x, 0.0))
    {
        a = 0.0;
        b = 1.0;
    }
    else
    {
        a = 1.0;
        b = -d_pos.y / d_pos.x;
    }

    const float c = -a * t_pos_b.y - b * t_pos_b.x;

    return {a, b, c};
}

Line Line::fromPointAndAngle(const Vec2 t_pos, const Angle t_ang)
{
    return fromTwoPoints(t_pos, t_pos + t_ang.toUnitVec());
}

Line Line::fromSegment(LineSegment segment)
{
    return fromTwoPoints(segment.start, segment.end);
}

float Line::y(const float t_x) const
{
    if (almostEqual(a, 0.0))
    {
        return 0;
    }

    return -(b * t_x + c) / a;
}

float Line::x(const float t_y) const
{
    if (almostEqual(b, 0.0))
    {
        return 0;
    }

    return -(a * t_y + c) / b;
}

std::optional<Vec2> Line::intersect(const Line &t_line) const
{
    if (almostEqual(slope(), t_line.slope()))
    {
        return {};
    }
    else if (almostEqual(a, 0.0))
    {
        const float x = -c / b;
        return {{x, t_line.y(x)}};
    }
    else if (almostEqual(t_line.a, 0.0))
    {
        const float x = -t_line.c / t_line.b;
        return {{x, y(x)}};
    }
    else
    {
        const float x = (a * t_line.c - t_line.a * c) / (t_line.a * b - a * t_line.b);
        return {{x, y(x)}};
    }
}

// TODO: untested, and probably broken
std::vector<Vec2> Line::intersect(const Circle &t_circle) const
{
    // line:   x = -c/b (if a = 0)
    // circle: (x-t_circle.center.x)^2 + (y-t_circle.center.y)^2 = r^2,
    // fill in:(-c/b-t_circle.center.x)^2 + y^2 -2ky + t_circle.center.y^2 - r^2 = 0
    //         y^2 -2ky + (-c/b-t_circle.center.x)^2 + t_circle.center.y^2 - r^2 = 0
    // and determine solutions for y using abc-formula
    if (std::abs(a) < std::numeric_limits<float>::epsilon())
    {
        const auto sol = abcFormula(1, -2 * t_circle.center.y,
                                    ((-c / b) - t_circle.center.x) * ((-c / b) - t_circle.center.x) +
                                        t_circle.center.y * t_circle.center.y - t_circle.r * t_circle.r);

        return {{(-c / b), sol[0]}, {(-c / b), sol[1]}};
    }

    // ay + bx + c = 0 => y = -b/a x - c/a, with da = -b/a and db = -c/a
    // circle: (x-t_circle.center.x)^2 + (y-t_circle.center.y)^2 = r^2, with t_circle.center.x = center.x and
    // t_circle.center.y = center.y fill in:x^2 -2hx + t_circle.center.x^2 + (da*x-db)^2 -2k(da*x-db) +
    // t_circle.center.y^2 - r^2 = 0
    //         x^2 -2hx + t_circle.center.x^2 + da^2*x^2 + 2da*db*x + db^2 -2k*da*x -2k*db
    //                                                         + t_circle.center.y^2 - r^2 = 0
    //       (1+da^2)*x^2 + 2(da*db-t_circle.center.x-t_circle.center.y*da)*x + h2 + db^2  -2k*db + t_circle.center.y^2
    //       - r^2 = 0
    // and determine solutions for x using abc-formula
    // fill in x in original line equation to get y coordinate
    const float da = -b / a;
    const float db = -c / a;

    const float d_a = 1 + da * da;
    const float d_b = 2 * (da * db - t_circle.center.x - t_circle.center.y * da);
    const float d_c = t_circle.center.x * t_circle.center.x + db * db - 2 * t_circle.center.y * db +
                      t_circle.center.y * t_circle.center.y - t_circle.r * t_circle.r;

    const auto sol = abcFormula(d_a, d_b, d_c);

    return {{sol[0], da * sol[0] + db}, {sol[1], da * sol[1] + db}};
}

Line Line::tangentLine(const Vec2 t_pos) const
{
    return {b, -a, a * t_pos.x - b * t_pos.y};
}

Vec2 Line::closestPoint(const Vec2 t_pos) const
{
    const Line tangent = tangentLine(t_pos);
    return intersect(tangent).value();
}
float Line::distanceTo(Vec2 t_pos) const
{
    Vec2 close_point = this->closestPoint(t_pos);
    return close_point.distanceTo(t_pos);
}

} // namespace Tyr::Common
