#include "renderer.h"

namespace Tyr::Gui
{
Renderer::Renderer(Common::Vec2 _wSize, float _upScalingFactor) : robotArcAngle(50.f), m_window_border(8.f)
{
    m_w_size           = _wSize * _upScalingFactor;
    m_upscaling_factor = _upScalingFactor;
    main_rt            = LoadRenderTexture((int) m_w_size.x, (int) m_w_size.y);
    shader_rt          = LoadRenderTexture((int) m_w_size.x, (int) m_w_size.y);
    SetTextureFilter(main_rt.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(shader_rt.texture, TEXTURE_FILTER_BILINEAR);
}

void Renderer::initialize()
{
    std::array<float, 2> resolution = {static_cast<float>(m_w_size.x), static_cast<float>(m_w_size.y)};

    const std::filesystem::path data_dir(DATA_DIR);
    const std::filesystem::path vertex_path   = data_dir / "shaders/raylib_vertex.vs";
    const std::filesystem::path fragment_path = data_dir / "shaders/fxaa.fs";
    const std::filesystem::path font_path     = data_dir / "fonts/OpenSans-Bold.ttf";

    fxaaShader = LoadShader(vertex_path.string().c_str(), fragment_path.string().c_str());
    m_font     = LoadFont(font_path.string().c_str());
    SetShaderValue(fxaaShader, GetShaderLocation(fxaaShader, "resolution"), resolution.data(), SHADER_UNIFORM_VEC2);
}

Vector2 Renderer::ConvertSignedVecToPixelVec(Common::Vec2 _signedVec)
{
    _signedVec.y *= -1.;
    _signedVec  = _signedVec * this->m_zoom_scale + this->m_w_size / 2;
    Vector2 vec = {.x = _signedVec.x, .y = _signedVec.y};
    return vec;
}

int Renderer::ConvertRealityUnitToPixels(float _value)
{
    return _value * m_zoom_scale;
}

void Renderer::sortPointsClockwise(Common::Vec2 &_p1, Common::Vec2 &_p2, Common::Vec2 &_p3)
{
    float        area = (_p2.x - _p1.x) * (_p3.y - _p1.y) - (_p3.x - _p1.x) * (_p2.y - _p1.y);
    Common::Vec2 temp;
    if (area < 0)
    {
        temp = _p3;
        _p3  = _p2;
        _p2  = temp;
    }
}

void Renderer::calculateMousePos()
{
    m_avil_size.x             = ImGui::GetContentRegionAvail().x * m_upscaling_factor;
    m_avil_size.y             = ImGui::GetContentRegionAvail().y * m_upscaling_factor;
    Common::Vec2 window_scale = m_w_size / m_avil_size;
    m_mouse_pos.x             = ImGui::GetMousePos().x;
    m_mouse_pos.y             = ImGui::GetMousePos().y;
    m_mouse_pos.x = (m_mouse_pos.x - ImGui::GetWindowPos().x - m_window_border) / m_zoom_scale * m_upscaling_factor *
                        window_scale.x -
                    overallFieldSize.x / 2;
    m_mouse_pos.y = -1 * ((m_mouse_pos.y - ImGui::GetWindowPos().y - m_window_border) / m_zoom_scale *
                              m_upscaling_factor * window_scale.y -
                          overallFieldSize.y / 2);
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
    Vector2 v1   = ConvertSignedVecToPixelVec(rect.min);
    Vector2 v2   = ConvertSignedVecToPixelVec(rect.max);
    t_thickness  = t_thickness * m_upscaling_factor;
    float posX   = (v1.x < v2.x) ? v1.x : v2.x;
    float posY   = (v1.y < v2.y) ? v1.y : v2.y;
    float length = (v1.x < v2.x) ? v2.x - v1.x : v1.x - v2.x;
    float width  = (v1.y < v2.y) ? v2.y - v1.y : v1.y - v2.y;

    Rectangle ray_rect = {.x = posX, .y = posY, .width = length, .height = width};

    BeginTextureMode(main_rt);
    if (t_is_filled)
    {
        DrawRectangleRec(ray_rect, raylibColor(t_color));
    }
    else
    {
        DrawRectangleLinesEx(ray_rect, t_thickness, raylibColor(t_color));
    }
    EndTextureMode();
}

void Renderer::draw(Common::Line t_line, Common::Color t_color, float t_thickness)
{
    // TODO
}

void Renderer::draw(Common::LineSegment line_segment, Common::Color t_color, float t_thickness)
{
    t_thickness = t_thickness * m_upscaling_factor;
    Vector2 v1  = ConvertSignedVecToPixelVec(line_segment.start);
    Vector2 v2  = ConvertSignedVecToPixelVec(line_segment.end);

    BeginTextureMode(main_rt);
    DrawLineEx(v1, v2, t_thickness, raylibColor(t_color));
    BeginTextureMode(main_rt);
}

void Renderer::draw(Common::Circle circle, Common::Color t_color, bool t_is_filled, float t_thickness)
{
    Vector2 center = ConvertSignedVecToPixelVec(circle.center);
    float   _rad   = ConvertRealityUnitToPixels(circle.r);
    t_thickness    = t_thickness * m_upscaling_factor;

    BeginTextureMode(main_rt);
    if (t_is_filled)
    {
        DrawCircleV(center, _rad, raylibColor(t_color));
    }
    else
    {
        DrawRing(center, _rad - t_thickness, _rad, 0., 360., 100, raylibColor(t_color));
    }
    EndTextureMode();
}

void Renderer::drawCircleSector(Common::Circle circle, Common::Color t_color, float _startAngle, float _endAngle,
                                bool t_is_filled)
{
    Vector2 center = ConvertSignedVecToPixelVec(circle.center);
    float   _rad   = ConvertRealityUnitToPixels(circle.r);

    Vector2 p1 = {.x = center.x + _rad * cos(_startAngle * DEG2RAD), .y = center.y + _rad * sin(_startAngle * DEG2RAD)};
    Vector2 p2 = {.x = center.x + _rad * cos(_endAngle * DEG2RAD), .y = center.y + _rad * sin(_endAngle * DEG2RAD)};

    BeginTextureMode(main_rt);
    if (t_is_filled)
    {
        DrawCircleSector(center, _rad, _startAngle, _endAngle, 200, raylibColor(t_color));
        DrawTriangle(center, p1, p2, raylibColor(t_color));
    }
    else
    {
        DrawCircleSectorLines(center, _rad, _startAngle, _endAngle, 500, raylibColor(t_color));
        DrawTriangleLines(center, p1, p2, raylibColor(t_color));
    }
    EndTextureMode();
}

void Renderer::draw(Common::Triangle triangle, Common::Color t_color, bool t_is_filled, float t_thickness)
{
    Vector2 v1 = ConvertSignedVecToPixelVec(triangle.corner[0]);
    Vector2 v2 = ConvertSignedVecToPixelVec(triangle.corner[1]);
    Vector2 v3 = ConvertSignedVecToPixelVec(triangle.corner[2]);

    BeginTextureMode(main_rt);
    if (t_is_filled)
    {
        DrawTriangle(v1, v2, v3, raylibColor(t_color));
    }
    else
    {
        // TODO: thickness
        DrawTriangleLines(v1, v2, v3, raylibColor(t_color));
    }
    EndTextureMode();
}

void Renderer::drawText(Common::Vec2 _pos, const std::string &_str, int _fontSize, Common::Color t_color)
{
    Vector2 pos = ConvertSignedVecToPixelVec(_pos);
    BeginTextureMode(main_rt);
    DrawTextEx(m_font, _str.c_str(), pos, _fontSize * m_upscaling_factor, 0., raylibColor(t_color));
    EndTextureMode();
}

void Renderer::CalculateZoom()
{
    bool         x     = this->m_w_size.x < this->m_w_size.y;
    Common::Vec2 ratio = Common::Vec2(this->m_w_size.x / overallFieldSize.x, this->m_w_size.y / overallFieldSize.y);
    this->m_zoom_scale = x ? (ratio.x > ratio.y ? ratio.x : ratio.y) : (ratio.x > ratio.y ? ratio.y : ratio.x);
}

void Renderer::applyShader()
{
    BeginTextureMode(shader_rt);
    BeginShaderMode(fxaaShader);
    DrawTexture(main_rt.texture, 0, 0, WHITE);
    EndShaderMode();
    EndTextureMode();
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
    draw(t_world.ball, true);
    for (const auto &robot : t_world.own_robot)
        draw(robot);
    for (const auto &robot : t_world.opp_robot)
        draw(robot);
}

void Renderer::draw(const Common::FieldState &t_field)
{
    BeginTextureMode(this->main_rt);
    ClearBackground(GREEN);
    EndTextureMode();

    this->overallFieldSize.x = t_field.width * 2.0f + 4.0f * t_field.boundary_width;
    this->overallFieldSize.y = t_field.height * 2.0f + 4 * t_field.boundary_width;

    CalculateZoom();
    calculateMousePos();

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

    draw(Common::Rect{fieldWallStartPoint, fieldWallEndPoint}, Common::Color::brown(), false, 3);
    draw(Common::Rect{fieldEndPoint, fieldStartPoint}, Common::Color::white(), false);
    draw(Common::Rect{ourGoalEndPoint, ourGoalStartPoint}, Common::Color::white(), false);
    draw(Common::Rect{oppGoalStartPoint, oppGoalEndPoint}, Common::Color::white(), false);
    draw(Common::Rect{ourPenaltyStartPoint, ourPenaltyEndPoint}, Common::Color::white(), false);
    draw(Common::Rect{oppenaltyStartPoint, oppPenaltyEndPoint}, Common::Color::white(), false);
    draw(Common::LineSegment{centerLineStartPoint, centerLineEndPoint}, Common::Color::white());
    draw(Common::Circle{fieldCenter, centerCircleRad}, Common::Color::white(), false);

    drawText(fieldWallEndPoint + Common::Vec2(-800., 300.), "GUI FPS: " + std::to_string(GetFPS()), 14,
             Common::Color::black());

    static bool         clicked = false;
    static Common::Vec2 clicked_pos(0, 0);

    if (ImGui::IsMouseClicked(0))
    {
        clicked     = true;
        clicked_pos = m_mouse_pos;
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
    return m_mouse_pos;
}
} // namespace Tyr::Gui
