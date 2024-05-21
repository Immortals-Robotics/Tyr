#pragma once

namespace Tyr::Gui
{

inline void Renderer::draw(const Common::Vec2 &point, const Common::Color &t_color, const float t_thickness)
{
    static constexpr float cross_size = 50.0f;

    const Common::LineSegment line_a = Common::LineSegment{
        point + Common::Vec2{-cross_size, -cross_size},
        point + Common::Vec2{cross_size, cross_size},
    };

    const Common::LineSegment line_b = Common::LineSegment{
        point + Common::Vec2{-cross_size, cross_size},
        point + Common::Vec2{cross_size, -cross_size},
    };

    draw(line_a, t_color, t_thickness);
    draw(line_b, t_color, t_thickness);
}

inline void Renderer::draw(const Common::Rect &t_rect, const Common::Color &t_color, bool t_is_filled,
                           float t_thickness)
{
    const Rectangle rect = mirrorY(static_cast<Rectangle>(t_rect));

    if (t_is_filled)
    {
        DrawRectangleRec(rect, t_color);
    }
    else
    {
        DrawRectangleLinesEx(rect, t_thickness, t_color);
    }
}

inline void Renderer::draw([[maybe_unused]] const Common::Line &t_line, [[maybe_unused]] const Common::Color &t_color,
                           [[maybe_unused]] float t_thickness)
{
    Common::logWarning("Line drawing is not implemented.");
}

inline void Renderer::draw(const Common::LineSegment &t_line, const Common::Color &t_color, float t_thickness)
{
    const Vector2 start = mirrorY(static_cast<Vector2>(t_line.start));
    const Vector2 end   = mirrorY(static_cast<Vector2>(t_line.end));

    DrawLineEx(start, end, t_thickness, t_color);
}

inline void Renderer::draw(const Common::Circle &t_circle, const Common::Color &t_color, bool t_is_filled,
                           float t_thickness)
{
    Vector2 center = mirrorY(static_cast<Vector2>(t_circle.center));

    if (t_is_filled)
    {
        DrawCircleV(center, t_circle.r, t_color);
    }
    else
    {
        DrawRing(center, t_circle.r - t_thickness, t_circle.r, 0., 360., 30, t_color);
    }
}

inline void Renderer::drawCircleSector(const Common::Circle &t_circle, const Common::Color &t_color,
                                       const float t_angle_start, const float t_angle_end, const bool t_is_filled)
{
    Vector2 center = mirrorY(static_cast<Vector2>(t_circle.center));

    Vector2 p1 =
        Vector2Add(center, static_cast<Vector2>(Common::Angle::fromDeg(t_angle_start).toUnitVec() * t_circle.r));
    Vector2 p2 = Vector2Add(center, static_cast<Vector2>(Common::Angle::fromDeg(t_angle_end).toUnitVec() * t_circle.r));

    if (t_is_filled)
    {
        DrawCircleSector(center, t_circle.r, t_angle_start, t_angle_end, 20, t_color);
        DrawTriangle(center, p1, p2, t_color);
    }
    else
    {
        DrawCircleSectorLines(center, t_circle.r, t_angle_start, t_angle_end, 20, t_color);
        DrawTriangleLines(center, p1, p2, t_color);
    }
}

inline void Renderer::draw(const Common::Triangle &t_triangle, const Common::Color &t_color, bool t_is_filled,
                           [[maybe_unused]] float t_thickness)
{
    Vector2 v1 = mirrorY(static_cast<Vector2>(t_triangle.corner[0]));
    Vector2 v2 = mirrorY(static_cast<Vector2>(t_triangle.corner[1]));
    Vector2 v3 = mirrorY(static_cast<Vector2>(t_triangle.corner[2]));

    if (t_is_filled)
    {
        DrawTriangle(v1, v2, v3, t_color);
    }
    else
    {
        // TODO: thickness
        DrawTriangleLines(v1, v2, v3, t_color);
    }
}

inline void Renderer::draw(const Common::Vec2 &t_pos, const std::string &t_text, const Common::Color &t_color,
                           const float t_font_size)
{
    const Common::Vec2 text_size = Common::Vec2{MeasureTextEx(m_font, t_text.c_str(), t_font_size, 0.0f)};

    const Vector2 pos = Vector2Subtract(mirrorY(static_cast<Vector2>(t_pos)), static_cast<Vector2>((text_size / 2.0f)));
    DrawTextEx(m_font, t_text.c_str(), pos, t_font_size, 0., t_color);
}
} // namespace Tyr::Gui
