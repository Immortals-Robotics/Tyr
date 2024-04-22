#include "FieldRenderer.h"

#include "../utility/vector_helper.h"
#include "../utility/colors.h"

void FieldRenderer::DrawFieldLegacy(const Protos::RoboCup2014Legacy::Geometry::SSL_GeometryFieldSize &data) const
{
	// Field

	auto lFieldBorderStartPos = ImVec2(-this->overallFieldSize.x / 2.f, +this->overallFieldSize.y / 2.f);
	auto lFieldBorderEndPos = ImVec2(+this->overallFieldSize.x / 2.f, -this->overallFieldSize.y / 2.f);

	this->drawList->AddRectFilled(
		lFieldBorderStartPos,
		lFieldBorderEndPos,
		IMC_FIELD_GREEN);

	auto lFieldStartPos = lFieldBorderStartPos + ImVec2(data.boundary_width(), -data.boundary_width());
	auto lFieldEndPos = lFieldStartPos + ImVec2(data.field_length(), -data.field_width());
	auto lFieldCenter = (lFieldStartPos + lFieldEndPos) / 2.f;

	// Field border
	this->drawList->AddRect(
		lFieldStartPos,
		lFieldEndPos,
		IMC_WHITE);

	this->drawList->PathClear();
	this->drawList->PathLineTo(lFieldBorderStartPos + ImVec2(data.boundary_width() - data.referee_width(), -data.boundary_width() + data.referee_width()));
	this->drawList->PathLineTo(lFieldBorderStartPos + ImVec2(data.boundary_width() + data.field_length() + data.referee_width(), -data.boundary_width() + data.referee_width()));
	this->drawList->PathLineTo(lFieldBorderStartPos + ImVec2(data.boundary_width() + data.field_length() + data.referee_width(), -data.boundary_width() - data.field_width() - data.referee_width()));
	this->drawList->PathLineTo(lFieldBorderStartPos + ImVec2(data.boundary_width() - data.referee_width(), -data.boundary_width() - data.field_width() - data.referee_width()));
	this->drawList->PathStroke(IMC_BLACK, true, 5.f);

	// Midline
	this->drawList->AddLine(
		ImVec2(lFieldCenter.x, lFieldStartPos.y),
		ImVec2(lFieldCenter.x, lFieldEndPos.y),
		IMC_WHITE);
	this->drawList->AddCircle(
		lFieldCenter,
		data.center_circle_radius(),
		IMC_WHITE,
		30);

	// TODO: Penalty area
	this->drawList->PathClear();
	this->drawList->PathArcTo(
		ImVec2(lFieldStartPos.x, lFieldCenter.y + data.defense_stretch() / 2),
		data.defense_radius(),
		-0.5f * IM_PI,
		0.f);
	this->drawList->PathLineTo(ImVec2(lFieldStartPos.x + data.defense_radius(),
									  lFieldCenter.y - data.defense_stretch() / 2));
	this->drawList->PathArcTo(
		ImVec2(lFieldStartPos.x, lFieldCenter.y - data.defense_stretch() / 2),
		data.defense_radius(),
		0.f,
		0.5f * IM_PI);
	this->drawList->PathStroke(IMC_WHITE, false);

	this->drawList->PathClear();
	this->drawList->PathArcTo(
		ImVec2(lFieldEndPos.x, lFieldCenter.y + data.defense_stretch() / 2),
		data.defense_radius(),
		-0.5f * IM_PI,
		-1.f * IM_PI);
	this->drawList->PathLineTo(ImVec2(lFieldEndPos.x - data.defense_radius(),
									  lFieldCenter.y - data.defense_stretch() / 2));
	this->drawList->PathArcTo(
		ImVec2(lFieldEndPos.x, lFieldCenter.y - data.defense_stretch() / 2),
		data.defense_radius(),
		-1.f * IM_PI,
		-1.5f * IM_PI);
	this->drawList->PathStroke(IMC_WHITE, false);

	this->drawList->PathClear();
	this->drawList->PathLineTo(ImVec2(lFieldStartPos.x, lFieldCenter.y - data.goal_width() / 2));
	this->drawList->PathLineTo(ImVec2(lFieldStartPos.x - data.goal_depth(), lFieldCenter.y - data.goal_width() / 2));
	this->drawList->PathLineTo(ImVec2(lFieldStartPos.x - data.goal_depth(), lFieldCenter.y + data.goal_width() / 2));
	this->drawList->PathLineTo(ImVec2(lFieldStartPos.x, lFieldCenter.y + data.goal_width() / 2));
	this->drawList->PathStroke(IMC_WHITE, false, 2.f);

	this->drawList->PathClear();
	this->drawList->PathLineTo(ImVec2(lFieldEndPos.x, lFieldCenter.y - data.goal_width() / 2));
	this->drawList->PathLineTo(ImVec2(lFieldEndPos.x + data.goal_depth(), lFieldCenter.y - data.goal_width() / 2));
	this->drawList->PathLineTo(ImVec2(lFieldEndPos.x + data.goal_depth(), lFieldCenter.y + data.goal_width() / 2));
	this->drawList->PathLineTo(ImVec2(lFieldEndPos.x, lFieldCenter.y + data.goal_width() / 2));
	this->drawList->PathStroke(IMC_WHITE, false, 2.f);
}
void FieldRenderer::DrawCameraCalibrations(const google::protobuf::RepeatedPtrField<Protos::SSL_GeometryCameraCalibration> &data) const
{
}