#include "FieldRendererRaylib.h"
#include "raylib.h"

void VisualizationRenderer::DrawRobots(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionRobot> &data, TeamColor color)
{
    auto lTextColor = color == Yellow ? BLACK : WHITE;
	auto lBaseColor = color == Yellow ? YELLOW : BLUE;
	
    for (auto robot : data)
	{
		auto position = ImVec2(robot.x(), robot.y());
        DrawCircleSectorVec(position, this->robotRadius * 10, lBaseColor, this->robotArcAngle - robot.orientation(), 360. - this->robotArcAngle - robot.orientation(), true);
	}
}