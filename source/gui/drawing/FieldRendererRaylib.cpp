#include "FieldRendererRaylib.h"
#include <cmath>
#include "raylib.h"


VisualizationRenderer::VisualizationRenderer(ImVec2 _wSize) : ballRadius(21.5f),
								 robotRadius(90.f),
								 robotArcAngle(50.f)
{
    wSize = _wSize;
    visualisaionTexture = LoadRenderTexture((int)wSize.x, (int)wSize.y);
    // SetTextureFilter(visualisaionTexture.texture, TEXTURE_FILTER_BILINEAR);
}

ImVec2 VisualizationRenderer::ConvertSignedVecToPixelVec(ImVec2 _signedVec)
{
    return _signedVec * this->zoomScale + this->wSize / 2;
}

int VisualizationRenderer::ConvertRealityUnitToPixels(float _value)
{
    return _value * zoomScale;
}

void VisualizationRenderer::DrawRectVec(ImVec2 _v1, ImVec2 _v2, Color _color, bool _isFilled, float _thikness)
{
    _v1 = ConvertSignedVecToPixelVec(_v1);
    _v2 = ConvertSignedVecToPixelVec(_v2);
    float posX = (_v1.x < _v2.x) ? _v1.x : _v2.x;
    float posY = (_v1.y < _v2.y) ? _v1.y : _v2.y;
    float length = (_v1.x < _v2.x) ? _v2.x - _v1.x : _v1.x - _v2.x;
    float width = (_v1.y < _v2.y) ? _v2.y - _v1.y : _v1.y - _v2.y;

    Rectangle rect = {.width = length, .height = width, .x = posX, .y = posY};

    if (_isFilled) {
        DrawRectangleRec(rect, _color);
    } else {
        DrawRectangleLinesEx(rect, _thikness, _color);
    }
}

void VisualizationRenderer::DrawLineVec(ImVec2 _v1, ImVec2 _v2, Color _color, float _thikness)
{
    _v1 = ConvertSignedVecToPixelVec(_v1);
    _v2 = ConvertSignedVecToPixelVec(_v2);
    Vector2 v1 = {.x = _v1.x, .y = _v1.y};
    Vector2 v2 = {.x = _v2.x, .y = _v2.y};

    DrawLineEx(v1, v2, _thikness, _color);
}

void VisualizationRenderer::DrawCircleVec(ImVec2 _center, float _rad, Color _color, bool _isFilled)
{
    _center = ConvertSignedVecToPixelVec(_center);
    _rad = ConvertRealityUnitToPixels(_rad);
    if (_isFilled) {
        DrawCircle(_center.x, _center.y, _rad, _color);
    } else {
        DrawCircleLines(_center.x, _center.y, _rad, _color);
    }
}

void VisualizationRenderer::DrawCircleSectorVec(ImVec2 _center, float _rad, Color _color, float _startAngle, float _endAngle, bool _isFilled)
{
    _center = ConvertSignedVecToPixelVec(_center);
    _rad = ConvertRealityUnitToPixels(_rad);
    Vector2 center = {.x = _center.x, .y = _center.y};
    Vector2 p1 = {.x = center.x + _rad * cos(_startAngle * DEG2RAD), .y = center.y + _rad * sin(_startAngle * DEG2RAD)};
    Vector2 p2 = {.x = center.x + _rad * cos(_endAngle * DEG2RAD), .y = center.y + _rad * sin(_endAngle * DEG2RAD)};

    if (_isFilled) {
        DrawCircleSector(center, _rad, _startAngle, _endAngle, 50, _color);
        DrawTriangle(center, p1, p2, _color);
    } else {
        DrawCircleSectorLines(center, _rad, _startAngle, _endAngle, 500, _color);
        DrawTriangleLines(center, p1, p2, _color);
    }
}

void VisualizationRenderer::CalculateZoom()
{
    bool x = this->wSize.x < this->wSize.y;
	ImVec2 ratio = ImVec2(this->wSize.x / overallFieldSize.x, this->wSize.y / overallFieldSize.y);
	this->zoomScale = x ? (ratio.x > ratio.y ? ratio.x : ratio.y) : (ratio.x > ratio.y ? ratio.y : ratio.x);
}




void VisualizationRenderer::StartDraw()
{
    BeginTextureMode(visualisaionTexture);
    ClearBackground(GREEN);
}

void VisualizationRenderer::EndDraw()
{
    // EndShaderMode();
    EndTextureMode();
}

void VisualizationRenderer::DrawField(const Protos::SSL_GeometryFieldSize &data)
{
    this->overallFieldSize.x = data.field_length() + 4 * (data.boundary_width());
	this->overallFieldSize.y = data.field_width() + 4 * (data.boundary_width());
    
	CalculateZoom();

    ImVec2 fieldWallStartPoint = ImVec2(data.field_length() / -2 - data.boundary_width(), data.field_width() / -2 - data.boundary_width());
    ImVec2 fieldWallEndPoint = ImVec2(data.field_length() / 2 + data.boundary_width(), data.field_width() / 2 + data.boundary_width());
    ImVec2 fieldStartPoint = ImVec2(data.field_length() / -2, data.field_width() / -2);
    ImVec2 fieldEndPoint = ImVec2(data.field_length() / 2, data.field_width() / 2);
    ImVec2 fieldCenter = ImVec2(0,0);
    

    ImVec2 ourGoalStartPoint = ImVec2(data.field_length() / -2 - data.goal_depth(), data.goal_width() / 2);
    ImVec2 ourGoalEndPoint = ImVec2(data.field_length() / -2, data.goal_width() / -2);
    ImVec2 oppGoalStartPoint = ourGoalStartPoint * -1;
    ImVec2 oppGoalEndPoint = ourGoalEndPoint * -1;

    ImVec2 ourPenaltyStartPoint = ImVec2(data.field_length() / -2, data.penalty_area_width() / 2);
    ImVec2 ourPenaltyEndPoint = ImVec2(data.field_length() / -2 + data.penalty_area_depth(), data.penalty_area_width() / -2);

    ImVec2 oppenaltyStartPoint = ourPenaltyStartPoint * -1;
    ImVec2 oppPenaltyEndPoint = ourPenaltyEndPoint * -1;

    ImVec2 centerLineStartPoint = ImVec2(0, data.field_width() / -2);
    ImVec2 centerLineEndPoint = centerLineStartPoint * -1;

    int centerCircleRad = data.center_circle_radius();
    

    DrawRectVec(fieldWallStartPoint, fieldWallEndPoint, BROWN, false, 3);
    DrawRectVec(fieldEndPoint, fieldStartPoint, WHITE, false);
    DrawRectVec(ourGoalEndPoint,ourGoalStartPoint, WHITE, false);
    DrawRectVec(oppGoalStartPoint,oppGoalEndPoint, WHITE, false);
    DrawRectVec(ourPenaltyStartPoint,ourPenaltyEndPoint, WHITE, false);
    DrawRectVec(oppenaltyStartPoint,oppPenaltyEndPoint, WHITE, false);
    DrawLineVec(centerLineStartPoint, centerLineEndPoint, WHITE);
    DrawCircleVec(fieldCenter, centerCircleRad, WHITE, false);
}