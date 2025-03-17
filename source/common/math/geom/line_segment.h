#pragma once

namespace Immortals::Common
{
struct LineSegment
{
    LineSegment(const Vec2 t_start, const Vec2 t_end) : start(t_start), end(t_end)
    {}

    explicit LineSegment(const Protos::Immortals::LineSegment &t_line)
        : LineSegment(Vec2{t_line.start()}, Vec2{t_line.end()})
    {}

    void fillProto(Protos::Immortals::LineSegment *const t_line) const
    {
        start.fillProto(t_line->mutable_start());
        end.fillProto(t_line->mutable_end());
    }

    double length() const
    {
        return (end - start).length();
    }

    double distanceTo(const Vec2 &t_point) const
    {
        double t = (t_point - start).dot(end - start) / (end - start).dot(end - start);

        if (t < 0)
        {
            return (t_point - start).length();
        }
        else if (t > 1)
        {
            return (t_point - end).length();
        }
        else
        {
            return (t_point - (start + (end - start) * t)).length();
        }
    }

    Vec2 closestPoint(const Vec2 &t_point) const
    {
        double t = (t_point - start).dot(end - start) / (end - start).dot(end - start);
        if (t < 0) {
            return start;
        }
        else if (t > 1) {
            return end;
        }
        else {
            return start + (end - start) * t;
        }
    }

    Vec2 start;
    Vec2 end;
};
} // namespace Immortals::Common
