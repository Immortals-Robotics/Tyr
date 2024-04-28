#include "renderer.h"

namespace Tyr::Gui
{
void Renderer::drawBalls(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionBall> &data)
{
    for (const auto &ball : data)
    {
        drawBall(ball);
    }
}

void Renderer::drawBall(const Protos::SSL_DetectionBall &ball)
{
    auto position = Common::Vec2(ball.x(), ball.y());
    drawCircle({position, ballRadius}, ORANGE, true);
    drawCircle({position, ballRadius + 10.0f}, BLACK, false, 1.0f);
}

} // namespace Tyr::Gui
