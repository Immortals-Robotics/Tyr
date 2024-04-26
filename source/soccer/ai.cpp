#include "ai.h"

namespace Tyr::Soccer
{
void Ai::AddDebugPoint(const Common::vec2 &p, const Common::CommonColor _color)
{
    Common::debug().drawPoint(p);
}

void Ai::AddDebugLine(const Common::vec2 &p1, const Common::vec2 &p2, const Common::CommonColor _color)
{
    Common::debug().drawLineSegment(p1, p2);
}

void Ai::AddDebugRect(const Common::vec2 &p, const float w, const float h, const Common::CommonColor _color)
{
    Common::debug().drawRect(p, w, h);
}

void Ai::AddDebugCircle(const Common::vec2 &p, const float r, const Common::CommonColor _color)
{
    Common::debug().drawCircle(p, r, "", Common::Color::createColor(_color));
}
} // namespace Tyr::Soccer
