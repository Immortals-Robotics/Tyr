#include "visualization_renderer.h"

namespace Tyr::Gui
{
VisualizationRenderer::VisualizationRenderer(ImVec2 _wSize, float _upScalingFactor)
    : ballRadius(21.5f), robotRadius(90.f), robotArcAngle(50.f)
{
    wSize                      = _wSize * _upScalingFactor;
    upScalingFactor            = _upScalingFactor;
    visualizaionTexture        = LoadRenderTexture((int) wSize.x, (int) wSize.y);
    shaderVisualizationTexture = LoadRenderTexture((int) wSize.x, (int) wSize.y);
    SetTextureFilter(visualizaionTexture.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(shaderVisualizationTexture.texture, TEXTURE_FILTER_BILINEAR);
}

void VisualizationRenderer::init()
{
    std::array<float, 2> resolution = {static_cast<float>(wSize.x), static_cast<float>(wSize.y)};

    fxaaShader        = LoadShader("shaders/raylib_vertex.vs", "shaders/fxaa.fs");
    visualizationFont = LoadFont("fonts/OpenSans-Bold.ttf");
    SetShaderValue(fxaaShader, GetShaderLocation(fxaaShader, "resolution"), resolution.data(), SHADER_UNIFORM_VEC2);
}

Vector2 VisualizationRenderer::ConvertSignedVecToPixelVec(ImVec2 _signedVec)
{
    _signedVec.y *= -1.;
    _signedVec  = _signedVec * this->zoomScale + this->wSize / 2;
    Vector2 vec = {.x = _signedVec.x, .y = _signedVec.y};
    return vec;
}

int VisualizationRenderer::ConvertRealityUnitToPixels(float _value)
{
    return _value * zoomScale;
}

void VisualizationRenderer::DrawRectVec(ImVec2 _v1, ImVec2 _v2, Color _color, bool _isFilled, float _thickness,
                                        unsigned char _transparency)
{
    Vector2 v1     = ConvertSignedVecToPixelVec(_v1);
    Vector2 v2     = ConvertSignedVecToPixelVec(_v2);
    _thickness     = _thickness * upScalingFactor;
    float posX     = (v1.x < v2.x) ? v1.x : v2.x;
    float posY     = (v1.y < v2.y) ? v1.y : v2.y;
    float length   = (v1.x < v2.x) ? v2.x - v1.x : v1.x - v2.x;
    float width    = (v1.y < v2.y) ? v2.y - v1.y : v1.y - v2.y;
    _color.a       = _transparency;
    Rectangle rect = {.x = posX, .y = posY, .width = length, .height = width};

    BeginTextureMode(visualizaionTexture);
    if (_isFilled)
    {
        DrawRectangleRec(rect, _color);
    }
    else
    {
        DrawRectangleLinesEx(rect, _thickness, _color);
    }
    EndTextureMode();
}

void VisualizationRenderer::DrawLineVec(ImVec2 _v1, ImVec2 _v2, Color _color, float _thickness,
                                        unsigned char _transparency)
{
    _thickness = _thickness * upScalingFactor;
    Vector2 v1 = ConvertSignedVecToPixelVec(_v1);
    Vector2 v2 = ConvertSignedVecToPixelVec(_v2);
    _color.a   = _transparency;
    BeginTextureMode(visualizaionTexture);
    DrawLineEx(v1, v2, _thickness, _color);
    BeginTextureMode(visualizaionTexture);
}

void VisualizationRenderer::DrawCircleVec(ImVec2 _center, float _rad, Color _color, bool _isFilled, float _thickness,
                                          unsigned char _transparency)
{
    Vector2 center = ConvertSignedVecToPixelVec(_center);
    _rad           = ConvertRealityUnitToPixels(_rad);
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

void VisualizationRenderer::DrawCircleSectorVec(ImVec2 _center, float _rad, Color _color, float _startAngle,
                                                float _endAngle, bool _isFilled, unsigned char _transparency)
{
    Vector2 center = ConvertSignedVecToPixelVec(_center);
    _rad           = ConvertRealityUnitToPixels(_rad);
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

void VisualizationRenderer::DrawTextVec(ImVec2 _pos, std::string _str, int _fontSize, Color _color,
                                        unsigned char _transparency)
{
    Vector2 pos = ConvertSignedVecToPixelVec(_pos);
    BeginTextureMode(visualizaionTexture);
    DrawTextEx(visualizationFont, _str.c_str(), pos, _fontSize * upScalingFactor, 0., _color);
    EndTextureMode();
}

void VisualizationRenderer::CalculateZoom()
{
    bool   x        = this->wSize.x < this->wSize.y;
    ImVec2 ratio    = ImVec2(this->wSize.x / overallFieldSize.x, this->wSize.y / overallFieldSize.y);
    this->zoomScale = x ? (ratio.x > ratio.y ? ratio.x : ratio.y) : (ratio.x > ratio.y ? ratio.y : ratio.x);
}

void VisualizationRenderer::ApplyShader()
{
    BeginTextureMode(shaderVisualizationTexture);
    BeginShaderMode(fxaaShader);
    DrawTexture(visualizaionTexture.texture, 0, 0, WHITE);
    EndShaderMode();
    EndTextureMode();
}

void VisualizationRenderer::DrawField(const Protos::SSL_GeometryFieldSize &data)
{
    BeginTextureMode(this->visualizaionTexture);
    ClearBackground(GREEN);
    EndTextureMode();

    this->overallFieldSize.x = data.field_length() + 4 * (data.boundary_width());
    this->overallFieldSize.y = data.field_width() + 4 * (data.boundary_width());

    CalculateZoom();

    ImVec2 fieldWallStartPoint =
        ImVec2(data.field_length() / -2 - data.boundary_width(), data.field_width() / -2 - data.boundary_width());
    ImVec2 fieldWallEndPoint =
        ImVec2(data.field_length() / 2 + data.boundary_width(), data.field_width() / 2 + data.boundary_width());
    ImVec2 fieldStartPoint = ImVec2(data.field_length() / -2, data.field_width() / -2);
    ImVec2 fieldEndPoint   = ImVec2(data.field_length() / 2, data.field_width() / 2);
    ImVec2 fieldCenter     = ImVec2(0, 0);

    ImVec2 ourGoalStartPoint = ImVec2(data.field_length() / -2 - data.goal_depth(), data.goal_width() / 2);
    ImVec2 ourGoalEndPoint   = ImVec2(data.field_length() / -2, data.goal_width() / -2);
    ImVec2 oppGoalStartPoint = ourGoalStartPoint * -1;
    ImVec2 oppGoalEndPoint   = ourGoalEndPoint * -1;

    ImVec2 ourPenaltyStartPoint = ImVec2(data.field_length() / -2, data.penalty_area_width() / 2);
    ImVec2 ourPenaltyEndPoint =
        ImVec2(data.field_length() / -2 + data.penalty_area_depth(), data.penalty_area_width() / -2);

    ImVec2 oppenaltyStartPoint = ourPenaltyStartPoint * -1;
    ImVec2 oppPenaltyEndPoint  = ourPenaltyEndPoint * -1;

    ImVec2 centerLineStartPoint = ImVec2(0, data.field_width() / -2);
    ImVec2 centerLineEndPoint   = centerLineStartPoint * -1;

    int centerCircleRad = data.center_circle_radius();

    DrawRectVec(fieldWallStartPoint, fieldWallEndPoint, BROWN, false, 3);
    DrawRectVec(fieldEndPoint, fieldStartPoint, WHITE, false);
    DrawRectVec(ourGoalEndPoint, ourGoalStartPoint, WHITE, false);
    DrawRectVec(oppGoalStartPoint, oppGoalEndPoint, WHITE, false);
    DrawRectVec(ourPenaltyStartPoint, ourPenaltyEndPoint, WHITE, false);
    DrawRectVec(oppenaltyStartPoint, oppPenaltyEndPoint, WHITE, false);
    DrawLineVec(centerLineStartPoint, centerLineEndPoint, WHITE);
    DrawCircleVec(fieldCenter, centerCircleRad, WHITE, false);
    DrawTextVec(fieldWallEndPoint + ImVec2(-800., 300.), "GUI FPS: " + std::to_string(GetFPS()), 14, BLACK);
}
} // namespace Tyr::Gui
