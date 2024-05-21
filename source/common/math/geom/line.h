#pragma once

#include "line_segment.h"

namespace Tyr::Common
{
struct Circle;

struct Line
{
    // ay + bx + c = 0
    Line(const float t_a, const float t_b, const float t_c) : a(t_a), b(t_b), c(t_c)
    {}

    explicit Line(const Protos::Immortals::Line &t_line) : Line(t_line.a(), t_line.b(), t_line.c())
    {}

    void fillProto(Protos::Immortals::Line *const t_line) const
    {
        t_line->set_a(a);
        t_line->set_b(b);
        t_line->set_c(c);
    }

    static Line fromTwoPoints(const Vec2 t_pos_a, const Vec2 t_pos_b)
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

    static Line fromPointAndAngle(const Vec2 t_pos, const Angle t_ang)
    {
        return fromTwoPoints(t_pos, t_pos + t_ang.toUnitVec());
    }

    static Line fromSegment(const LineSegment t_segment)
    {
        return fromTwoPoints(t_segment.start, t_segment.end);
    }

    [[nodiscard]] float slope() const
    {
        return -b / a;
    }

    [[nodiscard]] float y(const float t_x) const
    {
        if (almostEqual(a, 0.0))
        {
            return 0;
        }

        return -(b * t_x + c) / a;
    }

    [[nodiscard]] float x(const float t_y) const
    {
        if (almostEqual(b, 0.0))
        {
            return 0;
        }

        return -(a * t_y + c) / b;
    }

    // This method performs the abc formula (Pythagoras' Theorem) on the given parameters.
    static std::vector<float> abcFormula(float t_a, float t_b, float t_c);

    [[nodiscard]] std::optional<Vec2> intersect(const Line &t_line) const;

    [[nodiscard]] std::optional<Vec2> intersect(const LineSegment &t_line) const;

    [[nodiscard]] std::vector<Vec2> intersect(const Circle &t_circle) const;

    [[nodiscard]] Line tangentLine(const Vec2 t_pos) const
    {
        return {b, -a, a * t_pos.x - b * t_pos.y};
    }

    [[nodiscard]] Vec2 closestPoint(const Vec2 t_pos) const
    {
        const Line tangent = tangentLine(t_pos);
        return intersect(tangent).value();
    }

    [[nodiscard]] float distanceTo(const Vec2 t_pos) const
    {
        const Vec2 close_point = this->closestPoint(t_pos);
        return close_point.distanceTo(t_pos);
    }

    float a = 0.0f;
    float b = 0.0f;
    float c = 0.0f;
};
} // namespace Tyr::Common
