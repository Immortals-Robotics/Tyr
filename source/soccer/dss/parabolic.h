#pragma once

namespace Tyr::Soccer
{
class Parabolic
{
private:
    static bool  HasCollision(const Parabolic &a, const Parabolic &b, const float r, const float t);
    static float Distance(const Parabolic &a, const Parabolic &b, const float t);

public:
    Common::vec2 a;
    Common::vec2 v;
    Common::vec2 p;

    float t0;
    float t1;

    static bool HaveOverlap(const Parabolic &a, const Parabolic &b, const float r);
    static bool HasStaticOverlap(const Parabolic &a);

    Common::vec2 Evaluate(const float t) const;
    Common::vec2 EvaluateDerivative(const float t) const;
};
} // namespace Tyr::Soccer
