#include "visualization_renderer.h"

namespace Tyr::Gui
{
void VisualizationRenderer::DrawBalls(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionBall> &data)
{
    for (auto ball : data)
    {
        auto position = ImVec2(ball.x(), ball.y());
        DrawCircleVec(position, ballRadius, ORANGE, true);
        DrawCircleVec(position, ballRadius + 10., BLACK, false, 1.);
    }
}
} // namespace Tyr::Gui
