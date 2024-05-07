#include "renderer.h"

namespace Tyr::Gui
{
void Renderer::drawShapesUdp(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug::Draw> &draws)
{
    for (const auto &draw : draws)
    {
        switch (draw.shape_case())
        {
        case Protos::Immortals::Debug::Draw::kPoint:
            this->draw(draw.point().pos(), draw.color(), draw.filled(), draw.thickness());
            break;
        case Protos::Immortals::Debug::Draw::kLine:
            this->draw(draw.line(), draw.color(), draw.thickness());
            break;
        case Protos::Immortals::Debug::Draw::kLineSegment:
            this->draw(draw.line_segment(), draw.color(), draw.thickness());
            break;
        case Protos::Immortals::Debug::Draw::kRect:
            this->draw(draw.rect(), draw.color(), draw.filled(), draw.thickness());
            break;
        case Protos::Immortals::Debug::Draw::kCircle:
            this->draw(draw.circle(), draw.color(), draw.filled(), draw.thickness());
            break;
        case Protos::Immortals::Debug::Draw::kTriangle:
            this->draw(draw.triangle(), draw.color(), draw.filled(), draw.thickness());
            break;

        default:
            Common::logWarning("Unsupported shape: {}", (int) draw.shape_case());
            break;
        }
    }
}

} // namespace Tyr::Gui
