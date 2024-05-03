#include "renderer.h"

namespace Tyr::Gui
{
void Renderer::drawCirclesUdp(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug_Circle> &circles)
{
    for (const auto &circle : circles)
    {
        auto position = Common::Vec2(circle.x(), circle.y());
        auto rad      = static_cast<float>(circle.r());

        Color color = {.r = static_cast<unsigned char>(circle.col_r()),
                       .g = static_cast<unsigned char>(circle.col_g()),
                       .b = static_cast<unsigned char>(circle.col_b()),
                       .a = 255};

        drawCircle({position, rad}, color, true, 255);
    }
}

void Renderer::drawRectsUdp(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug_Rect> &rects)
{
    for (const auto &rect : rects)
    {
        auto p1 = Common::Vec2(rect.x1(), rect.y1());
        auto p2 = Common::Vec2(rect.x2(), rect.y2());

        Common::Rect draw_rect(p1, p2);

        Color color = {.r = static_cast<unsigned char>(rect.col_r()),
                       .g = static_cast<unsigned char>(rect.col_g()),
                       .b = static_cast<unsigned char>(rect.col_b()),
                       .a = 255};

        drawRect(draw_rect, color, false, 1.f, 255);
    }
}

void Renderer::drawPointsUdp(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug_Point> &points)
{
    for (const auto &point : points)
    {
        auto position = Common::Vec2(point.x(), point.y());

        Color color = {.r = static_cast<unsigned char>(point.col_r()),
                       .g = static_cast<unsigned char>(point.col_g()),
                       .b = static_cast<unsigned char>(point.col_b()),
                       .a = 255};

        drawCircle({position, 20}, color, true, 255);
    }
}

void Renderer::drawLinesUdp(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug_Line> &lines)
{
    for (const auto &line : lines)
    {
        Common::LineSegment line_segment;

        line_segment.start = Common::Vec2(line.x1(), line.y1());
        line_segment.end   = Common::Vec2(line.x2(), line.y2());

        Color color = {.r = static_cast<unsigned char>(line.col_r()),
                       .g = static_cast<unsigned char>(line.col_g()),
                       .b = static_cast<unsigned char>(line.col_b()),
                       .a = 255};

        drawLineSegment(line_segment, color);
    }
}

} // namespace Tyr::Gui
