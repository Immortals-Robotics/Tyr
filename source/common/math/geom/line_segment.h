#pragma once

namespace Tyr::Common
{
struct LineSegment
{
    inline LineSegment(const Vec2 t_start, const Vec2 t_end) : start(t_start), end(t_end)
    {}

    inline LineSegment(const Protos::Immortals::LineSegment &t_line) : LineSegment(t_line.start(), t_line.end())
    {}

    inline void fillProto(Protos::Immortals::LineSegment *const t_line) const
    {
        start.fillProto(t_line->mutable_start());
        end.fillProto(t_line->mutable_end());
    }

    Vec2 start;
    Vec2 end;
};
} // namespace Tyr::Common
