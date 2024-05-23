#pragma once

namespace Tyr::Gui
{

inline void Renderer::draw(const Common::Vec2 &t_point, const Common::Color &t_color, const float t_thickness) const
{
    static constexpr float cross_size = 50.0f;

    const auto line_a = Common::LineSegment{
        t_point + Common::Vec2{-cross_size, -cross_size},
        t_point + Common::Vec2{cross_size, cross_size},
    };

    const auto line_b = Common::LineSegment{
        t_point + Common::Vec2{-cross_size, cross_size},
        t_point + Common::Vec2{cross_size, -cross_size},
    };

    draw(line_a, t_color, t_thickness);
    draw(line_b, t_color, t_thickness);
}

inline void Renderer::draw(const Common::Rect &t_rect, const Common::Color &t_color, const bool t_is_filled,
                           const float t_thickness) const
{
    const Rectangle rect = mirrorY(static_cast<Rectangle>(t_rect));

    if (t_is_filled)
    {
        DrawRectangleRec(rect, static_cast<Color>(t_color));
    }
    else
    {
        DrawRectangleLinesEx(rect, t_thickness, static_cast<Color>(t_color));
    }
}

inline void Renderer::draw([[maybe_unused]] const Common::Line &t_line, [[maybe_unused]] const Common::Color &t_color,
                           [[maybe_unused]] float t_thickness) const
{
    Common::logWarning("Line drawing is not implemented.");
}

inline void Renderer::draw(const Common::LineSegment &t_line, const Common::Color &t_color,
                           const float t_thickness) const
{
    const Vector2 start = mirrorY(static_cast<Vector2>(t_line.start));
    const Vector2 end   = mirrorY(static_cast<Vector2>(t_line.end));

    DrawLineEx(start, end, t_thickness, static_cast<Color>(t_color));
}

inline void Renderer::draw(const Common::Circle &t_circle, const Common::Color &t_color, const bool t_is_filled,
                           const float t_thickness) const
{
    Vector2 center = mirrorY(static_cast<Vector2>(t_circle.center));

    if (t_is_filled)
    {
        DrawCircleV(center, t_circle.r, static_cast<Color>(t_color));
    }
    else
    {
        DrawRing(center, t_circle.r - t_thickness, t_circle.r, 0., 360., 30, static_cast<Color>(t_color));
    }
}

inline void Renderer::drawCircleSector(const Common::Circle &t_circle, const Common::Color &t_color,
                                       const float t_angle_start, const float t_angle_end, const bool t_is_filled) const
{
    Vector2 center = mirrorY(static_cast<Vector2>(t_circle.center));

    Vector2 p1 =
        Vector2Add(center, static_cast<Vector2>(Common::Angle::fromDeg(t_angle_start).toUnitVec() * t_circle.r));
    Vector2 p2 = Vector2Add(center, static_cast<Vector2>(Common::Angle::fromDeg(t_angle_end).toUnitVec() * t_circle.r));

    if (t_is_filled)
    {
        DrawCircleSector(center, t_circle.r, t_angle_start, t_angle_end, 20, static_cast<Color>(t_color));
        DrawTriangle(center, p1, p2, static_cast<Color>(t_color));
    }
    else
    {
        DrawCircleSectorLines(center, t_circle.r, t_angle_start, t_angle_end, 20, static_cast<Color>(t_color));
        DrawTriangleLines(center, p1, p2, static_cast<Color>(t_color));
    }
}

inline void Renderer::draw(const Common::Triangle &t_triangle, const Common::Color &t_color, const bool t_is_filled,
                           [[maybe_unused]] float t_thickness) const
{
    Vector2 v1 = mirrorY(static_cast<Vector2>(t_triangle.corner[0]));
    Vector2 v2 = mirrorY(static_cast<Vector2>(t_triangle.corner[1]));
    Vector2 v3 = mirrorY(static_cast<Vector2>(t_triangle.corner[2]));

    if (t_is_filled)
    {
        DrawTriangle(v1, v2, v3, static_cast<Color>(t_color));
    }
    else
    {
        // TODO: thickness
        DrawTriangleLines(v1, v2, v3, static_cast<Color>(t_color));
    }
}

inline void Renderer::draw(const Common::Vec2 &t_pos, const std::string &t_text, const Common::Color &t_color,
                           const float t_font_size) const
{
    const Common::Vec2 text_size = Common::Vec2{MeasureTextEx(m_font, t_text.c_str(), t_font_size, 0.0f)};

    const Vector2 pos = Vector2Subtract(mirrorY(static_cast<Vector2>(t_pos)), static_cast<Vector2>((text_size / 2.0f)));
    DrawTextEx(m_font, t_text.c_str(), pos, t_font_size, 0., static_cast<Color>(t_color));
}
} // namespace Tyr::Gui
