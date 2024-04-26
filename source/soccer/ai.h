#pragma once

namespace Tyr::Soccer
{
class Ai
{
protected:
    bool debugDraw;
    void AddDebugPoint(const Common::vec2 &p, const Common::CommonColor _color = Common::White);
    void AddDebugLine(const Common::vec2 &p1, const Common::vec2 &p2, const Common::CommonColor _color = Common::White);
    void AddDebugRect(const Common::vec2 &p, const float w, const float h, const Common::CommonColor _color = Common::White);
    void AddDebugCircle(const Common::vec2 &p, const float r, const Common::CommonColor _color = Common::White);

public:
    virtual void Process(Common::WorldState *worldState) = 0;
};
} // namespace Tyr::Soccer
