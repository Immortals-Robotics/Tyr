#pragma once

#include "color.h"
#include "source_location.h"

namespace Tyr::Common::Debug
{
struct Draw
{
    SourceLocation source;

    Color color     = Color::white();
    bool  filled    = true;
    float thickness = 1.0f;

    std::variant<Vec2, Line, LineSegment, Rect, Circle, Triangle> shape;

    Draw() = default;

    Draw(const Protos::Immortals::Debug::Draw &t_draw, const StringMap &t_strings)
    {
        source    = SourceLocation{t_draw.source(), t_strings};
        color     = Color{t_draw.color()};
        filled    = t_draw.filled();
        thickness = t_draw.thickness();

        switch (t_draw.shape_case())
        {
        case Protos::Immortals::Debug::Draw::kPoint:
            shape = Vec2{t_draw.point().pos()};
            break;
        case Protos::Immortals::Debug::Draw::kLine:
            shape = Line{t_draw.line()};
            break;
        case Protos::Immortals::Debug::Draw::kLineSegment:
            shape = LineSegment{t_draw.line_segment()};
            break;
        case Protos::Immortals::Debug::Draw::kRect:
            shape = Rect{t_draw.rect()};
            break;
        case Protos::Immortals::Debug::Draw::kCircle:
            shape = Circle{t_draw.circle()};
            break;
        case Protos::Immortals::Debug::Draw::kTriangle:
            shape = Triangle{t_draw.triangle()};
            break;
        default:
            logWarning("Unsupported shape type: {}", static_cast<int>(t_draw.shape_case()));
            break;
        }
    }

    void fillProto(Protos::Immortals::Debug::Draw *t_draw, StringMap *t_strings) const
    {
        source.fillProto(t_draw->mutable_source(), t_strings);
        color.fillProto(t_draw->mutable_color());
        t_draw->set_filled(filled);
        t_draw->set_thickness(thickness);

        if (const auto point = std::get_if<Vec2>(&shape); point)
            point->fillProto(t_draw->mutable_point()->mutable_pos());
        else if (const auto line = std::get_if<Line>(&shape); line)
            line->fillProto(t_draw->mutable_line());
        else if (const auto segment = std::get_if<LineSegment>(&shape); segment)
            segment->fillProto(t_draw->mutable_line_segment());
        else if (const auto rect = std::get_if<Rect>(&shape); rect)
            rect->fillProto(t_draw->mutable_rect());
        else if (const auto circle = std::get_if<Circle>(&shape); circle)
            circle->fillProto(t_draw->mutable_circle());
        else if (const auto triangle = std::get_if<Triangle>(&shape); triangle)
            triangle->fillProto(t_draw->mutable_triangle());
        else
            logWarning("Unsupported shape type: {}", shape.index());
    }
};
} // namespace Tyr::Common::Debug
