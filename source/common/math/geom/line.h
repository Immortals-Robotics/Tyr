#pragma once

#include "../angle.h"
#include "../vector.h"

namespace Tyr::Common
{
struct Circle;
struct LineSegment;

struct Line
{
public:
    // ay + bx + c = 0
    Line(float t_a, float t_b, float t_c);

    Line(const Protos::Immortals::Line &t_line) : Line(t_line.a(), t_line.b(), t_line.c())
    {}

    void fillProto(Protos::Immortals::Line *const t_line) const
    {
        t_line->set_a(a);
        t_line->set_b(b);
        t_line->set_c(c);
    }

    static Line fromTwoPoints(Vec2 t_pos_a, Vec2 t_pos_b);
    static Line fromPointAndAngle(Vec2 t_pos, Angle t_ang);
    static Line fromSegment(LineSegment segment);

    [[nodiscard]] float slope() const
    {
        return -b / a;
    }

    [[nodiscard]] float y(float t_x) const;
    [[nodiscard]] float x(float t_y) const;

    [[nodiscard]] std::optional<Vec2> intersect(const Line &t_line) const;

    [[nodiscard]] std::vector<Vec2> intersect(const Circle &t_circle) const;

    [[nodiscard]] Line  tangentLine(Vec2 t_pos) const;
    [[nodiscard]] Vec2  closestPoint(Vec2 t_pos) const;
    [[nodiscard]] float distanceTo(Vec2 t_pos) const;

    float a = 0.0f;
    float b = 0.0f;
    float c = 0.0f;
};
} // namespace Tyr::Common
