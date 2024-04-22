#include "FieldRenderer.h"
#include "../utility/colors.h"

void FieldRenderer::DrawBalls(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionBall> &data) const
{
	for (auto ball : data)
	{
		this->drawList->AddCircleFilled(ImVec2(ball.x(), ball.y()), ballRadius, IMC_ORANGE, 10);
		this->drawList->AddCircle(ImVec2(ball.x(), ball.y()), ballRadius + 10.f, IMC_BLACK, 10);
	}
}

void FieldRenderer::DrawBalls(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug_Circle> &data) const
{
	for (auto ball : data)
	{
		this->drawList->AddCircleFilled(ImVec2(ball.x(), ball.y()), ballRadius, IMC_ORANGE, 10);
		this->drawList->AddCircle(ImVec2(ball.x(), ball.y()), ballRadius + 10.f, IMC_BLACK, 10);
	}
}