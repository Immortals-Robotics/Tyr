#include "circle.h"

namespace Tyr::Common
{
std::vector<Vec2> Circle::intersect(const Circle &t_other) const
{
    // first calculate distance between two centers circles P0 and P1.
    const Vec2  d_vec = t_other.center - center;
    const float d_mag = d_vec.length();

    // normalize differences
    const Vec2 d_normal = d_vec.normalized();

    // a is distance between p0 and point that is the intersection point P2
    // that intersects P0-P1 and the line that crosses the two intersection
    // points P3 and P4.
    // Define two triangles: P0,P2,P3 and P1,P2,P3.
    // with distances a, h, r0 and b, h, r1 with d = a + b
    // We know a^2 + h^2 = r0^2 and b^2 + h^2 = r1^2 which then gives
    // a^2 + r1^2 - b^2 = r0^2 with d = a + b ==> a^2 + r1^2 - (d-a)^2 = r0^2
    // ==> r0^2 + d^2 - r1^2 / 2*d
    const float a = (r * r + d_mag * d_mag - t_other.r * t_other.r) / (2.0f * d_mag);

    // h is then a^2 + h^2 = r0^2 ==> h = sqrt( r0^2 - a^2 )
    const float arg = r * r - a * a;
    const float h   = arg > 0.0f ? sqrt(arg) : 0.0f;

    // First calculate P2
    const Vec2 p2 = center + d_normal * a;

    const Vec2 point1 = {p2.x - h * d_normal.y, p2.y + h * d_normal.x};

    const Vec2 point2 = {p2.x + h * d_normal.y, p2.y - h * d_normal.x};

    if (arg < 0.0f)
    {
        return {};
    }
    else if (arg == 0.0f)
    {
        return {point1};
    }
    else
    {
        return {point1, point2};
    }
}

float Circle::intersectionArea(const Circle &t_other) const
{
    const Vec2  d_vec = t_other.center - center;
    const float d_mag = d_vec.length(); // dist between two centers

    if (d_mag > t_other.r + r) // circles do not intersect
    {
        return 0.0f;
    }
    if (d_mag <= std::abs(t_other.r - r)) // one totally in the other
    {
        // return area smallest circle
        const float min_r = std::min(t_other.r, r);
        return std::numbers::pi * min_r * min_r;
    }

    const auto intersection = intersect(t_other);

    if (intersection.size() != 2)
    {
        return 0.0f;
    }

    // the intersection area of two circles can be divided into two segments:
    // left and right of the line between the two intersection points p1 and p2.
    // The outside area of each segment can be calculated by taking the part
    // of the circle pie excluding the triangle from the center to the
    // two intersection points.
    // The pie equals pi*r^2 * rad(2*ang) / 2*pi = 0.5*rad(2*ang)*r^2 with ang
    // the angle between the center c of the circle and one of the two
    // intersection points. Thus the angle between c and p1 and c and p3 where
    // p3 is the point that lies halfway between p1 and p2.
    // This can be calculated using ang = asin( d / r ) with d the distance
    // between p1 and p3 and r the radius of the circle.
    // The area of the triangle is 2*0.5*h*d.

    const Vec2 int_mid = (intersection[0] + intersection[1]) * 0.5f;

    const float d = (intersection[0] - int_mid).length();

    float area = 0.0f;
    {
        const float h   = (int_mid - center).length();
        const Angle ang = Angle::fromRad(std::asin(d / r));

        area += ang.rad() * r * r;
        area -= d * h;
    }

    {
        // and now for the other segment the same story
        const float h   = (int_mid - t_other.center).length();
        const Angle ang = Angle::fromRad(std::asin(d / t_other.r));

        area += ang.rad() * t_other.r * t_other.r;
        area -= d * h;
    }

    return area;
}

bool Circle::isCircleCross(const Vec2 t_point1, const Vec2 t_point2) const
{
    const float y1 = t_point1.y;
    const float x1 = t_point1.x;

    const float y2 = t_point2.y;
    const float x2 = t_point2.x;

    // general form of line equation is a * x + b * y + c = 0
    const float a = -(y2 - y1);
    const float b = (x2 - x1);
    const float c = -(a * x1 + b * y1);

    const float p = abs(a * center.x + b * center.y + c) / (sqrt(a * a + b * b));

    if (p > r)
        return false;

    const float v_a = -b;
    const float v_b = a;
    const float v_c = -v_b * center.y - v_a * center.x;

    if (v_a * x1 + v_b * y1 + v_c >= 0 && v_a * x2 + v_b * y2 + v_c >= 0)
    {
        if (inside(t_point1) || inside(t_point2))
            return true;
    }
    else if (v_a * x1 + v_b * y1 + v_c <= 0 && v_a * x2 + v_b * y2 + v_c <= 0)
    {
        if (inside(t_point1) || inside(t_point2))
        {
            return true;
        }
    }
    else
    {
        return true;
    }
    return false;
}
} // namespace Tyr::Common
