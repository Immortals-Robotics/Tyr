#include "renderer.h"

namespace Tyr::Gui
{
Renderer::Renderer(Common::Vec2 _wSize, float _upScalingFactor)
    : ballRadius(21.5f), robotRadius(90.f), robotArcAngle(50.f)
{
    wSize                      = _wSize * _upScalingFactor;
    upScalingFactor            = _upScalingFactor;
    visualizaionTexture        = LoadRenderTexture((int) wSize.x, (int) wSize.y);
    shaderVisualizationTexture = LoadRenderTexture((int) wSize.x, (int) wSize.y);
    SetTextureFilter(visualizaionTexture.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(shaderVisualizationTexture.texture, TEXTURE_FILTER_BILINEAR);
}

void Renderer::init()
{
    std::array<float, 2> resolution = {static_cast<float>(wSize.x), static_cast<float>(wSize.y)};

    fxaaShader        = LoadShader("shaders/raylib_vertex.vs", "shaders/fxaa.fs");
    visualizationFont = LoadFont("fonts/OpenSans-Bold.ttf");
    SetShaderValue(fxaaShader, GetShaderLocation(fxaaShader, "resolution"), resolution.data(), SHADER_UNIFORM_VEC2);
}

Vector2 Renderer::ConvertSignedVecToPixelVec(Common::Vec2 _signedVec)
{
    _signedVec.y *= -1.;
    _signedVec  = _signedVec * this->zoomScale + this->wSize / 2;
    Vector2 vec = {.x = _signedVec.x, .y = _signedVec.y};
    return vec;
}

int Renderer::ConvertRealityUnitToPixels(float _value)
{
    return _value * zoomScale;
}

void Renderer::drawRect(Common::Rect rect, Color _color, bool _isFilled, float _thickness, unsigned char _transparency)
{
    Vector2 v1   = ConvertSignedVecToPixelVec(rect.min);
    Vector2 v2   = ConvertSignedVecToPixelVec(rect.max);
    _thickness   = _thickness * upScalingFactor;
    float posX   = (v1.x < v2.x) ? v1.x : v2.x;
    float posY   = (v1.y < v2.y) ? v1.y : v2.y;
    float length = (v1.x < v2.x) ? v2.x - v1.x : v1.x - v2.x;
    float width  = (v1.y < v2.y) ? v2.y - v1.y : v1.y - v2.y;
    _color.a     = _transparency;

    Rectangle ray_rect = {.x = posX, .y = posY, .width = length, .height = width};

    BeginTextureMode(visualizaionTexture);
    if (_isFilled)
    {
        DrawRectangleRec(ray_rect, _color);
    }
    else
    {
        DrawRectangleLinesEx(ray_rect, _thickness, _color);
    }
    EndTextureMode();
}

void Renderer::drawLineSegment(Common::LineSegment line_segment, Color _color, float _thickness,
                               unsigned char _transparency)
{
    _thickness = _thickness * upScalingFactor;
    Vector2 v1 = ConvertSignedVecToPixelVec(line_segment.start);
    Vector2 v2 = ConvertSignedVecToPixelVec(line_segment.end);
    _color.a   = _transparency;
    BeginTextureMode(visualizaionTexture);
    DrawLineEx(v1, v2, _thickness, _color);
    BeginTextureMode(visualizaionTexture);
}

void Renderer::drawCircle(Common::Circle circle, Color _color, bool _isFilled, float _thickness,
                          unsigned char _transparency)
{
    Vector2 center = ConvertSignedVecToPixelVec(circle.center);
    float   _rad   = ConvertRealityUnitToPixels(circle.r);
    _thickness     = _thickness * upScalingFactor;
    _color.a       = _transparency;
    BeginTextureMode(visualizaionTexture);
    if (_isFilled)
    {
        DrawCircleV(center, _rad, _color);
    }
    else
    {
        DrawRing(center, _rad - _thickness, _rad, 0., 360., 100, _color);
    }
    EndTextureMode();
}

void Renderer::drawCircleSector(Common::Circle circle, Color _color, float _startAngle, float _endAngle, bool _isFilled,
                                unsigned char _transparency)
{
    Vector2 center = ConvertSignedVecToPixelVec(circle.center);
    float   _rad   = ConvertRealityUnitToPixels(circle.r);
    Vector2 p1 = {.x = center.x + _rad * cos(_startAngle * DEG2RAD), .y = center.y + _rad * sin(_startAngle * DEG2RAD)};
    Vector2 p2 = {.x = center.x + _rad * cos(_endAngle * DEG2RAD), .y = center.y + _rad * sin(_endAngle * DEG2RAD)};
    _color.a   = _transparency;
    BeginTextureMode(visualizaionTexture);
    if (_isFilled)
    {
        DrawCircleSector(center, _rad, _startAngle, _endAngle, 200, _color);
        DrawTriangle(center, p1, p2, _color);
    }
    else
    {
        DrawCircleSectorLines(center, _rad, _startAngle, _endAngle, 500, _color);
        DrawTriangleLines(center, p1, p2, _color);
    }
    EndTextureMode();
}

void Renderer::drawText(Common::Vec2 _pos, std::string _str, int _fontSize, Color _color, unsigned char _transparency)
{
    Vector2 pos = ConvertSignedVecToPixelVec(_pos);
    BeginTextureMode(visualizaionTexture);
    DrawTextEx(visualizationFont, _str.c_str(), pos, _fontSize * upScalingFactor, 0., _color);
    EndTextureMode();
}

void Renderer::CalculateZoom()
{
    bool         x     = this->wSize.x < this->wSize.y;
    Common::Vec2 ratio = Common::Vec2(this->wSize.x / overallFieldSize.x, this->wSize.y / overallFieldSize.y);
    this->zoomScale    = x ? (ratio.x > ratio.y ? ratio.x : ratio.y) : (ratio.x > ratio.y ? ratio.y : ratio.x);
}

void Renderer::applyShader()
{
    BeginTextureMode(shaderVisualizationTexture);
    BeginShaderMode(fxaaShader);
    DrawTexture(visualizaionTexture.texture, 0, 0, WHITE);
    EndShaderMode();
    EndTextureMode();
}

void Renderer::drawField(const Protos::SSL_GeometryFieldSize &data)
{
    BeginTextureMode(this->visualizaionTexture);
    ClearBackground(GREEN);
    EndTextureMode();

    this->overallFieldSize.x = data.field_length() + 4 * (data.boundary_width());
    this->overallFieldSize.y = data.field_width() + 4 * (data.boundary_width());

    CalculateZoom();

    Common::Vec2 fieldWallStartPoint =
        Common::Vec2(data.field_length() / -2 - data.boundary_width(), data.field_width() / -2 - data.boundary_width());
    Common::Vec2 fieldWallEndPoint =
        Common::Vec2(data.field_length() / 2 + data.boundary_width(), data.field_width() / 2 + data.boundary_width());
    Common::Vec2 fieldStartPoint = Common::Vec2(data.field_length() / -2, data.field_width() / -2);
    Common::Vec2 fieldEndPoint   = Common::Vec2(data.field_length() / 2, data.field_width() / 2);
    Common::Vec2 fieldCenter     = Common::Vec2(0, 0);

    Common::Vec2 ourGoalStartPoint = Common::Vec2(data.field_length() / -2 - data.goal_depth(), data.goal_width() / 2);
    Common::Vec2 ourGoalEndPoint   = Common::Vec2(data.field_length() / -2, data.goal_width() / -2);
    Common::Vec2 oppGoalStartPoint = ourGoalStartPoint * -1;
    Common::Vec2 oppGoalEndPoint   = ourGoalEndPoint * -1;

    Common::Vec2 ourPenaltyStartPoint = Common::Vec2(data.field_length() / -2, data.penalty_area_width() / 2);
    Common::Vec2 ourPenaltyEndPoint =
        Common::Vec2(data.field_length() / -2 + data.penalty_area_depth(), data.penalty_area_width() / -2);

    Common::Vec2 oppenaltyStartPoint = ourPenaltyStartPoint * -1;
    Common::Vec2 oppPenaltyEndPoint  = ourPenaltyEndPoint * -1;

    Common::Vec2 centerLineStartPoint = Common::Vec2(0, data.field_width() / -2);
    Common::Vec2 centerLineEndPoint   = centerLineStartPoint * -1;

    float centerCircleRad = data.center_circle_radius();

    drawRect({fieldWallStartPoint, fieldWallEndPoint}, BROWN, false, 3);
    drawRect({fieldEndPoint, fieldStartPoint}, WHITE, false);
    drawRect({ourGoalEndPoint, ourGoalStartPoint}, WHITE, false);
    drawRect({oppGoalStartPoint, oppGoalEndPoint}, WHITE, false);
    drawRect({ourPenaltyStartPoint, ourPenaltyEndPoint}, WHITE, false);
    drawRect({oppenaltyStartPoint, oppPenaltyEndPoint}, WHITE, false);
    drawLineSegment({centerLineStartPoint, centerLineEndPoint}, WHITE);
    drawCircle({fieldCenter, centerCircleRad}, WHITE, false);
    drawText(fieldWallEndPoint + Common::Vec2(-800., 300.), "GUI FPS: " + std::to_string(GetFPS()), 14, BLACK);
}
} // namespace Tyr::Gui
