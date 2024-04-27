#include "FieldRendererRaylib.h"
#include "raylib.h"
#define TEXT_OFFSET_X 50.
#define TEXT_OFFSET_Y 300.

void VisualizationRenderer::DrawRobots(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionRobot> &data, TeamColor color)
{
    auto lTextColor = color == Yellow ? BLACK : WHITE;
	auto lBaseColor = color == Yellow ? YELLOW : BLUE;
    for (auto robot : data)
	{
		auto position = ImVec2(robot.x(), robot.y());
		DrawCircleSectorVec(position, this->robotRadius, lBaseColor, this->robotArcAngle - robot.orientation() * RAD2DEG, 360. - this->robotArcAngle - robot.orientation() * RAD2DEG, true);
		DrawTextVec(position + ImVec2(TEXT_OFFSET_X, TEXT_OFFSET_Y), std::to_string(robot.robot_id()), 14, lTextColor);
	}
}