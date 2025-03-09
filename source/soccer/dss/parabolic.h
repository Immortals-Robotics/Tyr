#pragma once

namespace Tyr::Soccer
{
class ObstacleMap;
class Parabolic
{
private:
    static bool  HasCollision(const Parabolic &a, const Parabolic &b, float r, float t);
    static float Distance(const Parabolic &a, const Parabolic &b, float t);

public:
    Common::Vec2 a;
    Common::Vec2 v;
    Common::Vec2 p;

    float t0;
    float t1;

    static bool HaveOverlap(const Parabolic &a, const Parabolic &b, float r);
    static bool HasStaticOverlap(const Parabolic &a, const ObstacleMap &map);

    Common::Vec2 Evaluate(float t) const;
    Common::Vec2 EvaluateDerivative(float t) const;
};
} // namespace Tyr::Soccer
