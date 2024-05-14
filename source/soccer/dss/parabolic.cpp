#include "parabolic.h"
#include "../obstacle/map.h"

namespace Tyr::Soccer
{
bool Parabolic::HaveOverlap(const Parabolic &a, const Parabolic &b, const float r)
{
    const float t0 = std::min(a.t0, b.t0);
    const float t1 = std::min(a.t1, b.t1);

    if (t1 <= t0)
    {
        return false;
    }

    float check_t[6];
    int   check_t_count = 0;

    check_t[check_t_count++] = t0;
    check_t[check_t_count++] = t1;
    check_t[check_t_count++] = (t0 + t1) / 2.f;

    for (int t_idx = 0; t_idx < check_t_count; ++t_idx)
    {
        const float t = check_t[t_idx];
        if (HasCollision(a, b, r, t))
        {
            return true;
        }
    }

    return false;
}

bool Parabolic::HasStaticOverlap(const Parabolic &a)
{
    const float t0 = a.t0;
    const float t1 = a.t1;

    if (t1 <= t0)
    {
        return false;
    }

    float check_t[6];
    int   check_t_count = 0;

    check_t[check_t_count++] = t0;
    check_t[check_t_count++] = t1;
    check_t[check_t_count++] = (t0 + t1) / 2.f;

    for (int t_idx = 0; t_idx < check_t_count; ++t_idx)
    {
        const float        t = check_t[t_idx];
        const Common::Vec2 p = a.Evaluate(t);
        if (g_obs_map.isInObstacle(p))
        {
            return true;
        }
    }

    return false;
}

bool Parabolic::HasCollision(const Parabolic &a, const Parabolic &b, const float r, const float t)
{
    const float t0 = std::min(a.t0, b.t0);
    const float t1 = std::min(a.t1, b.t1);

    if (t1 <= t0 || t < t0 || t > t1)
    {
        return false;
    }

    const float dis = Distance(a, b, t);
    return dis <= r;
}

float Parabolic::Distance(const Parabolic &a, const Parabolic &b, const float t)
{
    const Common::Vec2 p_a = a.Evaluate(t);
    const Common::Vec2 p_b = b.Evaluate(t);
    return p_a.distanceTo(p_b);
}

Common::Vec2 Parabolic::Evaluate(const float t) const
{
    if (a.length() < 1e-10 && v.length() < 1e-10)
    {
        return p;
    }

    const float dt = t - t0;
    const float x  = p.x + (v.x * dt) + (a.x * (dt * dt * .5f));
    const float y  = p.y + (v.y * dt) + (a.y * (dt * dt * .5f));
    return Common::Vec2(x, y);
}

Common::Vec2 Parabolic::EvaluateDerivative(const float t) const
{
    if (a.length() < 1e-10)
    {
        return v;
    }

    const float dt = t - t0;
    const float x  = v.x + (a.x * dt);
    const float y  = v.y + (a.y * dt);
    return Common::Vec2(x, y);
}
} // namespace Tyr::Soccer
