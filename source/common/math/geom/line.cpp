#include "line.h"

namespace Immortals::Common
{
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

std::optional<Vec2> Line::intersect(const LineSegment &t_line) const
{
    const auto point = intersect(fromSegment(t_line));

    if (!point.has_value())
        return {};

    const float min_x = std::min(t_line.start.x, t_line.end.x);
    const float max_x = std::max(t_line.start.x, t_line.end.x);
    const float min_y = std::min(t_line.start.y, t_line.end.y);
    const float max_y = std::max(t_line.start.y, t_line.end.y);

    if ((point.value().x > min_x && point.value().x < max_x) || (point.value().y > min_y && point.value().y < max_y))
    {
        return point;
    }
    else
    {
        return {};
    }
}

std::vector<float> Line::abcFormula(const float t_a, const float t_b, const float t_c)
{
    // discriminant is b^2 - 4*a*c
    const float discr = t_b * t_b - 4 * t_a * t_c;

    if (almostEqual(discr, 0.0f))
    {
        // discriminant = 0: only one solution
        return {-t_b / (2 * t_a)};
    }
    else if (discr < 0)
    {
        // discriminant < 0: no solutions
        return {};
    }
    else
    {
        // discriminant > 0: two solutions
        const float discr_sq = std::sqrt(discr);
        return {(-t_b + discr_sq) / (2 * t_a), (-t_b - discr_sq) / (2 * t_a)};
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
    if (almostEqual(a, 0.0f))
    {
        const auto solutions = abcFormula(1, -2 * t_circle.center.y,
                                          ((-c / b) - t_circle.center.x) * ((-c / b) - t_circle.center.x) +
                                              t_circle.center.y * t_circle.center.y - t_circle.r * t_circle.r);

        std::vector<Vec2> answer{};
        answer.reserve(solutions.size());
        for (const auto solution : solutions)
            answer.emplace_back((-c / b), solution);
        return answer;
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

    const auto solutions = abcFormula(d_a, d_b, d_c);

    std::vector<Vec2> answer{};
    answer.reserve(solutions.size());
    for (const auto solution : solutions)
        answer.emplace_back(solution, da * solution + db);
    return answer;
}
} // namespace Immortals::Common
