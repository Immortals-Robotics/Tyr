#include "FieldRenderer.h"
#include "../utility/colors.h"

void FieldRenderer::DrawRobots(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionRobot> &data, TeamColor color) const
{
	auto lTextColor = color == Yellow ? IMC_BLACK : IMC_WHITE;
	auto lBaseColor = color == Yellow ? IMC_YELLOW : IMC_BLUE;

	for (auto robot : data)
	{
		auto position = ImVec2(robot.x(), robot.y());

		this->drawList->PathClear();
		this->drawList->PathArcTo(position, this->robotRadius, this->robotArcAngle - robot.orientation(), IM_2PI - this->robotArcAngle - robot.orientation(), 20);
		this->drawList->PathFill(lBaseColor);

		this->drawList->PathClear();
		this->drawList->PathArcTo(position, this->robotRadius, this->robotArcAngle - robot.orientation(), IM_2PI - this->robotArcAngle - robot.orientation(), 20);
		this->drawList->PathStroke(IMC_BLACK, true);

		char lNumberStr[3];
		sprintf(lNumberStr, "%x", robot.robot_id());

		this->drawList->AddText(
			ImGui::GetFont(),
			ImGui::GetFontSize(),
			position - ImVec2(this->robotRadius / 3.0f, -this->robotRadius / 1.75f),
			lTextColor,
			lNumberStr,
			NULL);

		this->drawList->AddRectFilled(
			ImVec2(position.x - this->robotRadius, position.y - this->robotRadius - 20.0f),
			ImVec2(position.x - this->robotRadius + robot.confidence() * 2 * this->robotRadius, position.y - this->robotRadius - 60.0f),
			IMC_CYAN);

		this->drawList->AddRect(
			ImVec2(position.x - this->robotRadius, position.y - this->robotRadius - 20.0f),
			ImVec2(position.x + this->robotRadius, position.y - this->robotRadius - 60.0f),
			IMC_BLACK);
	}
}
