#pragma once

#include "../utility/scaled_imdrawlist.h"

enum TeamColor
{
	Blue = 0,
	Yellow = 1,
};

class FieldRenderer
{
private:
	ImDrawListScaled *drawList;
	ImVec2 widgetPos;
	ImVec2 widgetSize;
	ImVec2 overallFieldSize;

	const float ballRadius;
	const float robotRadius;
	const float robotArcAngle;

	void CalculateZoom();

public:
	FieldRenderer();
	void SetDrawList(ImDrawList *const drawList);
	void SetWidgetProperties(const ImVec2 &pos, const ImVec2 &size);
	void SetFieldSizeLegacy(const Protos::RoboCup2014Legacy::Geometry::SSL_GeometryFieldSize &field);

	void DrawFieldLegacy(const Protos::RoboCup2014Legacy::Geometry::SSL_GeometryFieldSize &data) const;
	void DrawCameraCalibrations(const google::protobuf::RepeatedPtrField<Protos::SSL_GeometryCameraCalibration> &data) const;

	void DrawBalls(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug_Circle> &data) const;
	void DrawRobots(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug_Robot> &data) const;

	void DrawBalls(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionBall> &data) const;
	void DrawRobots(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionRobot> &data, TeamColor color) const;
};
