#include "renderer.h"

namespace Tyr::Gui
{
Renderer::Renderer()
{
    const std::filesystem::path data_dir(DATA_DIR);
    const std::filesystem::path font_path = data_dir / "fonts/open-sans-regular.ttf";

    m_font = LoadFont(font_path.string().c_str());
}

void Renderer::beginDraw(const Common::FieldState &t_field)
{
    const Common::Vec2 window_pos  = {ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y};
    const Common::Vec2 window_size = {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y};

    const Common::Vec2 offset = window_pos + window_size / 2.0f;

    const Common::Vec2 field_size = {t_field.width * 2.0f + 4.0f * t_field.boundary_width,
                                     t_field.height * 2.0f + 4.0f * t_field.boundary_width};

    const Common::Vec2 ratio = window_size / field_size;

    const float zoom = window_size.x < window_size.y ? (ratio.x > ratio.y ? ratio.x : ratio.y)
                                                     : (ratio.x > ratio.y ? ratio.y : ratio.x);

    m_camera = {
        .offset   = Vector2{offset.x, offset.y}, // Offset from the target
        .target   = Vector2{0.0f, 0.0f},         // Camera target (center of the screen)
        .rotation = 0.0f,                        // Camera rotation in degrees
        .zoom     = zoom,                        // Camera zoom (1.0f means no zoom)
    };

    BeginMode2D(m_camera);
    BeginScissorMode(window_pos.x, window_pos.y, window_size.x, window_size.y);
}

void Renderer::endDraw()
{
    EndScissorMode();
    EndMode2D();
}

Vector2 Renderer::ConvertSignedVecToPixelVec(Common::Vec2 _signedVec)
{
    return {.x = _signedVec.x, .y = -_signedVec.y};
}

static Color raylibColor(const Common::Color &t_color)
{
    return Color{
        .r = (unsigned char) t_color.r,
        .g = (unsigned char) t_color.g,
        .b = (unsigned char) t_color.b,
        .a = (unsigned char) t_color.a,
    };
}

void Renderer::draw(Common::Vec2 point, Common::Color t_color, bool t_is_filled, float t_thickness)
{
    // TODO: do sth better
    draw(Common::Circle(point, 50.0f), t_color, t_is_filled, t_thickness);
}

void Renderer::draw(Common::Rect rect, Common::Color t_color, bool t_is_filled, float t_thickness)
{
    Vector2 v1     = ConvertSignedVecToPixelVec(rect.min);
    Vector2 v2     = ConvertSignedVecToPixelVec(rect.max);
    float   posX   = (v1.x < v2.x) ? v1.x : v2.x;
    float   posY   = (v1.y < v2.y) ? v1.y : v2.y;
    float   length = (v1.x < v2.x) ? v2.x - v1.x : v1.x - v2.x;
    float   width  = (v1.y < v2.y) ? v2.y - v1.y : v1.y - v2.y;

    Rectangle ray_rect = {.x = posX, .y = posY, .width = length, .height = width};

    if (t_is_filled)
    {
        DrawRectangleRec(ray_rect, raylibColor(t_color));
    }
    else
    {
        DrawRectangleLinesEx(ray_rect, t_thickness, raylibColor(t_color));
    }
}

void Renderer::draw(Common::Line t_line, Common::Color t_color, float t_thickness)
{
    // TODO
}

void Renderer::draw(Common::LineSegment line_segment, Common::Color t_color, float t_thickness)
{
    Vector2 v1 = ConvertSignedVecToPixelVec(line_segment.start);
    Vector2 v2 = ConvertSignedVecToPixelVec(line_segment.end);

    DrawLineEx(v1, v2, t_thickness, raylibColor(t_color));
}

void Renderer::draw(Common::Circle circle, Common::Color t_color, bool t_is_filled, float t_thickness)
{
    Vector2 center = ConvertSignedVecToPixelVec(circle.center);

    if (t_is_filled)
    {
        DrawCircleV(center, circle.r, raylibColor(t_color));
    }
    else
    {
        DrawRing(center, circle.r - t_thickness, circle.r, 0., 360., 100, raylibColor(t_color));
    }
}

void Renderer::drawCircleSector(Common::Circle circle, Common::Color t_color, float _startAngle, float _endAngle,
                                bool t_is_filled)
{
    Vector2 center = ConvertSignedVecToPixelVec(circle.center);

    Vector2 p1 = {.x = center.x + circle.r * cos(_startAngle * DEG2RAD),
                  .y = center.y + circle.r * sin(_startAngle * DEG2RAD)};
    Vector2 p2 = {.x = center.x + circle.r * cos(_endAngle * DEG2RAD),
                  .y = center.y + circle.r * sin(_endAngle * DEG2RAD)};

    if (t_is_filled)
    {
        DrawCircleSector(center, circle.r, _startAngle, _endAngle, 20, raylibColor(t_color));
        DrawTriangle(center, p1, p2, raylibColor(t_color));
    }
    else
    {
        DrawCircleSectorLines(center, circle.r, _startAngle, _endAngle, 20, raylibColor(t_color));
        DrawTriangleLines(center, p1, p2, raylibColor(t_color));
    }
}

void Renderer::draw(Common::Triangle triangle, Common::Color t_color, bool t_is_filled, float t_thickness)
{
    Vector2 v1 = ConvertSignedVecToPixelVec(triangle.corner[0]);
    Vector2 v2 = ConvertSignedVecToPixelVec(triangle.corner[1]);
    Vector2 v3 = ConvertSignedVecToPixelVec(triangle.corner[2]);

    if (t_is_filled)
    {
        DrawTriangle(v1, v2, v3, raylibColor(t_color));
    }
    else
    {
        // TODO: thickness
        DrawTriangleLines(v1, v2, v3, raylibColor(t_color));
    }
}

void Renderer::drawText(Common::Vec2 _pos, const std::string &_str, int _fontSize, Common::Color t_color)
{
    Vector2 pos = ConvertSignedVecToPixelVec(_pos);
    DrawTextEx(m_font, _str.c_str(), pos, _fontSize, 0., raylibColor(t_color));
}

void Renderer::draw(const Common::RawWorldState &t_world)
{
    for (const auto &ball : t_world.balls)
        draw(ball);
    for (const auto &robot : t_world.yellow_robots)
        draw(robot);
    for (const auto &robot : t_world.blue_robots)
        draw(robot);
}

void Renderer::draw(const Common::WorldState &t_world)
{
    draw(t_world.ball);
    for (const auto &robot : t_world.own_robot)
        draw(robot);
    for (const auto &robot : t_world.opp_robot)
        draw(robot);
}

void Renderer::draw(const Common::FieldState &t_field)
{
    ClearBackground(GREEN);

    Common::Vec2 fieldWallStartPoint = Common::Vec2(t_field.width * 2.0f / -2 - t_field.boundary_width,
                                                    t_field.height * 2.0f / -2 - t_field.boundary_width);
    Common::Vec2 fieldWallEndPoint   = Common::Vec2(t_field.width * 2.0f / 2 + t_field.boundary_width,
                                                    t_field.height * 2.0f / 2 + t_field.boundary_width);
    Common::Vec2 fieldStartPoint     = Common::Vec2(t_field.width * 2.0f / -2, t_field.height * 2.0f / -2);
    Common::Vec2 fieldEndPoint       = Common::Vec2(t_field.width * 2.0f / 2, t_field.height * 2.0f / 2);
    Common::Vec2 fieldCenter         = Common::Vec2(0, 0);

    Common::Vec2 ourGoalStartPoint =
        Common::Vec2(t_field.width * 2.0f / -2 - t_field.goal_depth, t_field.goal_width / 2);
    Common::Vec2 ourGoalEndPoint   = Common::Vec2(t_field.width * 2.0f / -2, t_field.goal_width / -2);
    Common::Vec2 oppGoalStartPoint = ourGoalStartPoint * -1;
    Common::Vec2 oppGoalEndPoint   = ourGoalEndPoint * -1;

    Common::Vec2 ourPenaltyStartPoint = Common::Vec2(t_field.width * 2.0f / -2, t_field.penalty_area_width / 2);
    Common::Vec2 ourPenaltyEndPoint =
        Common::Vec2(t_field.width * 2.0f / -2 + t_field.penalty_area_depth, t_field.penalty_area_width / -2);

    Common::Vec2 oppenaltyStartPoint = ourPenaltyStartPoint * -1;
    Common::Vec2 oppPenaltyEndPoint  = ourPenaltyEndPoint * -1;

    Common::Vec2 centerLineStartPoint = Common::Vec2(0, t_field.height * 2.0f / -2);
    Common::Vec2 centerLineEndPoint   = centerLineStartPoint * -1;

    float centerCircleRad = t_field.center_circle_radius;

    draw(Common::Rect{fieldWallStartPoint, fieldWallEndPoint}, Common::Color::brown(), false, 70.0f);

    static constexpr float kLineWidth = 20.0f;

    draw(Common::Rect{fieldEndPoint, fieldStartPoint}, Common::Color::white(), false, kLineWidth);
    draw(Common::Rect{ourGoalEndPoint, ourGoalStartPoint}, Common::Color::white(), false, kLineWidth);
    draw(Common::Rect{oppGoalStartPoint, oppGoalEndPoint}, Common::Color::white(), false, kLineWidth);
    draw(Common::Rect{ourPenaltyStartPoint, ourPenaltyEndPoint}, Common::Color::white(), false, kLineWidth);
    draw(Common::Rect{oppenaltyStartPoint, oppPenaltyEndPoint}, Common::Color::white(), false, kLineWidth);
    draw(Common::LineSegment{centerLineStartPoint, centerLineEndPoint}, Common::Color::white(), kLineWidth);
    draw(Common::Circle{fieldCenter, centerCircleRad}, Common::Color::white(), false, kLineWidth);

    drawText(fieldWallEndPoint + Common::Vec2(-800., 300.), "GUI FPS: " + std::to_string(GetFPS()), 200,
             Common::Color::black());

    static bool         clicked = false;
    static Common::Vec2 clicked_pos(0, 0);

    if (ImGui::IsMouseClicked(0))
    {
        clicked     = true;
        clicked_pos = getMousePosition();
    }
    else if (ImGui::IsMouseClicked(1))
    {
        clicked = false;
    }

    if (clicked)
    {
        draw(Common::Circle{clicked_pos, 50}, Common::Color::red(), true, 170);
    }
}

Common::Vec2 Renderer::getMousePosition()
{
    const Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), m_camera);
    return {mouse_pos.x, -mouse_pos.y};
}
} // namespace Tyr::Gui
