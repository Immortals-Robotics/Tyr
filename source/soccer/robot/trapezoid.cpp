#include "trapezoid.h"

namespace Tyr::Soccer
{
inline float sign(float num)
{
    if (num >= 0.0f)
        return 1.0f;
    return -1.0f;
}

TrapezoidPlanner::TrapezoidPlanner()
{
    init(Common::Vec2(0.0f), Common::Vec2(0.0f), Common::Vec2(0.0f), 0.0f);
}

TrapezoidPlanner::TrapezoidPlanner(Common::Vec2 _max_acc, Common::Vec2 _max_dec, Common::Vec2 _max_spd, float _period)
{
    init(_max_acc, _max_dec, _max_spd, _period);
}

void TrapezoidPlanner::init(Common::Vec2 _max_acc, Common::Vec2 _max_dec, Common::Vec2 _max_spd, float _period)
{
    init_state  = NULL;
    final_state = NULL;

    max_acc = _max_acc;
    max_dec = _max_dec;
    max_spd = _max_spd;

    period = _period;

    oldAns = Common::Vec3(0.0f);
}

float TrapezoidPlanner::Plan1D(float d, float v, float tmp_max_spd, float tmp_max_acc, float tmp_max_dec)
{
    float ans = 0.0f;
    // If the current velocity has a different sign than the difference in positions, decelerate to a complete stop
    if (v * d < 0)
    {
        ans = std::fabs(v) - (1.0f * tmp_max_dec * period);
        if (ans < 0)
            ans = 0.0f;
        ans *= sign(v);
    }

    // If the current velocity will overshoot the target, decelerate to a complete stop
    else if ((v * v) / (2.0f * tmp_max_dec) > std::fabs(d))
    {
        ans = std::fabs(v) - (tmp_max_dec * period);
        if (ans < 0)
            ans = 0.0f;
        ans *= sign(v);
    }

    // If the current velocity exceeds the maximum, decelerate to the maximum.
    else if (std::fabs(v) > tmp_max_spd)
    {
        ans = std::fabs(v) - (tmp_max_dec * period);
        if (ans < tmp_max_spd)
            ans = tmp_max_spd;
        ans *= sign(v);
    }

    // Calculate a triangular velocity profile that will close the gap.
    else
    {
        ans = std::fabs(v) + (tmp_max_acc * period);

        // If the peak of the triangular profile exceeds the maximum speed, calculate a trapezoidal velocity profile.
        if (ans > tmp_max_spd)
            ans = tmp_max_spd;

        if ((ans * ans) / (2.0f * tmp_max_dec) > std::fabs(d))
        {
            ans = std::sqrt(2.0f * tmp_max_dec * std::fabs(d));
        }
        ans *= sign(d);
    }

    return ans;
}

Common::Vec3 TrapezoidPlanner::Plan(Common::RobotState *_init, Common::RobotState *_final)
{
    init_state  = _init;
    final_state = _final;

    if ((init_state == NULL) || (final_state == NULL))
        return Common::Vec3();

    Common::Vec3 ans = Common::Vec3();

    ans.z = (init_state->angle - final_state->angle).deg();
    ans.z = std::clamp(ans.z * 3.0f, -180.0f, 180.0f);

    if (std::fabs(oldAns.x - init_state->velocity.x) > 1000.0f)
        oldAns.x = (oldAns.x + init_state->velocity.x) / 2.0f;
    if (std::fabs(oldAns.y - init_state->velocity.y) > 1000.0f)
        oldAns.y = (oldAns.y + init_state->velocity.y) / 2.0f;

    float target_dis = init_state->Position.distanceTo(final_state->Position);

    Common::Vec2 tmp_max_speed;
    tmp_max_speed.x = max_spd.x * std::fabs(final_state->Position.x - init_state->Position.x) / target_dis;
    tmp_max_speed.y = max_spd.y * std::fabs(final_state->Position.y - init_state->Position.y) / target_dis;

    ans.x = Plan1D(final_state->Position.x - init_state->Position.x, oldAns.x, tmp_max_speed.x, max_acc.x, max_dec.x);
    ans.y = Plan1D(final_state->Position.y - init_state->Position.y, oldAns.y, tmp_max_speed.y, max_acc.y, max_dec.y);

    oldAns = ans;

    ans.x /= 42.5f;
    ans.y /= 42.5f;

    return ans;
}
} // namespace Tyr::Soccer
