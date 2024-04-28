#pragma once

namespace Tyr::Soccer
{
class Parabolic
{
private:
    static bool  HasCollision(const Parabolic &a, const Parabolic &b, const float r, const float t);
    static float Distance(const Parabolic &a, const Parabolic &b, const float t);

public:
    Common::Vec2 a;
    Common::Vec2 v;
    Common::Vec2 p;

    float t0;
    float t1;

    static bool HaveOverlap(const Parabolic &a, const Parabolic &b, const float r);
    static bool HasStaticOverlap(const Parabolic &a);

    Common::Vec2 Evaluate(const float t) const;
    Common::Vec2 EvaluateDerivative(const float t) const;
};
} // namespace Tyr::Soccer
