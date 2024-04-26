#pragma once

class VisualizationRenderer
{
private:
	const float ballRadius;
	const float robotRadius;
	const float robotArcAngle;
    ImVec2 wSize;
	ImVec2 overallFieldSize;
	float zoomScale;
	void CalculateZoom();
	ImVec2 ConvertSignedVecToPixelVec(ImVec2 _signedVec);
	int ConvertRealityUnitToPixels(float _value);
public:
	VisualizationRenderer(ImVec2 _wSize);
    RenderTexture visualisaionTexture;
	void DrawRectVec(ImVec2 _v1, ImVec2 _v2, Color _color, bool _isFilled, float _thikness = 1);
	void DrawCircleVec(ImVec2 _center, float _rad, Color _color, bool _isFilled);
	void DrawCircleSectorVec(ImVec2 _center, float _rad, Color _color, float _startAngle, float _endAngle, bool _isFilled);
	void DrawLineVec(ImVec2 _v1, ImVec2 _v2, Color _color, float _thikness = 1);


	void DrawField(const Protos::SSL_GeometryFieldSize &data);
	void DrawRobots(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionRobot> &data, TeamColor color);
	void StartDraw(void);
	void EndDraw(void);
};
