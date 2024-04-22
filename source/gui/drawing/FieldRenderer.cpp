#include "FieldRenderer.h"
#include "../utility/colors.h"

FieldRenderer::FieldRenderer() : ballRadius(21.5f),
								 robotRadius(90.f),
								 robotArcAngle(0.85)
{
	this->drawList = new ImDrawListScaled();
	// TODO: init field size according to the rules
	this->overallFieldSize = ImVec2(9600, 6600);
}

void FieldRenderer::SetDrawList(ImDrawList *const drawList)
{
	this->drawList->drawList = drawList;
}

void FieldRenderer::CalculateZoom()
{
	bool x = this->widgetSize.x < this->widgetSize.y;
	ImVec2 ratio = ImVec2(this->widgetSize.x / overallFieldSize.x, this->widgetSize.y / overallFieldSize.y);
	drawList->scale = x ? (ratio.x > ratio.y ? ratio.x : ratio.y) : (ratio.x > ratio.y ? ratio.y : ratio.x);
}

void FieldRenderer::SetWidgetProperties(const ImVec2 &pos, const ImVec2 &size)
{
	this->widgetPos = pos;
	this->drawList->offset = pos + size / 2.0f;
	this->widgetSize = size;

	CalculateZoom();
}

void FieldRenderer::SetFieldSizeLegacy(const Protos::RoboCup2014Legacy::Geometry::SSL_GeometryFieldSize &field)
{
	this->overallFieldSize.x = field.field_length() + 2 * (field.boundary_width());
	this->overallFieldSize.y = field.field_width() + 2 * (field.boundary_width());

	CalculateZoom();
}
