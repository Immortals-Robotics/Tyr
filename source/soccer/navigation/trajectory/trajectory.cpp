#include "trajectory.h"

namespace Tyr::Soccer
{
Trajectory Trajectory::MakeTrajectory(const Common::RobotState &state, const Common::Vec2 &a_acc, const float a_dec,
                                      const float a_dt)
{
    Trajectory trajectory;

    const float t0 = 0.f;

    const Common::Vec2 p0 = state.position;
    const Common::Vec2 v0 = state.velocity;
    const Common::Vec2 a0 = a_acc;

    const float t1 = t0 + a_dt;

    trajectory.acc.t0 = t0;
    trajectory.acc.t1 = t1;
    trajectory.acc.p  = p0;
    trajectory.acc.v  = v0;
    trajectory.acc.a  = a0;

    const Common::Vec2 p1 = trajectory.acc.Evaluate(t1);
    const Common::Vec2 v1 = trajectory.acc.EvaluateDerivative(t1);
    const Common::Vec2 a1 = v1.normalized() * (-a_dec);

    const float dec_dt = v1.length() / a_dec;
    const float t2     = t1 + dec_dt;

    trajectory.dec.t0 = t1;
    trajectory.dec.t1 = t2;
    trajectory.dec.p  = p1;
    trajectory.dec.v  = v1;
    trajectory.dec.a  = a1;

    const Common::Vec2 p2 = trajectory.dec.Evaluate(t2);
    const Common::Vec2 v2{};
    const Common::Vec2 a2{};

    trajectory.stopped.t0 = t2;
    trajectory.stopped.t1 = std::numeric_limits<float>::max();
    trajectory.stopped.p  = p2;
    trajectory.stopped.v  = v2;
    trajectory.stopped.a  = a2;

    return trajectory;
}

Trajectory Trajectory::MakeOpponentTrajectory(const Common::RobotState &state, const float a_dec)
{
    Trajectory trajectory;

    const float t0 = -1.f;

    const Common::Vec2 p0 = state.position;
    const Common::Vec2 v0 = state.velocity;
    const Common::Vec2 a0{};

    const float t1 = 0.f;

    trajectory.acc.t0 = t0;
    trajectory.acc.t1 = t1;
    trajectory.acc.p  = p0;
    trajectory.acc.v  = v0;
    trajectory.acc.a  = a0;

    const Common::Vec2 p1 = p0;
    const Common::Vec2 v1 = v0;
    const Common::Vec2 a1 = v1.normalized() * (-a_dec);

    const float dec_dt = v1.length() / a_dec;
    const float t2     = t1 + dec_dt;

    trajectory.dec.t0 = t1;
    trajectory.dec.t1 = t2;
    trajectory.dec.p  = p1;
    trajectory.dec.v  = v1;
    trajectory.dec.a  = a1;

    const Common::Vec2 p2 = trajectory.dec.Evaluate(t2);
    const Common::Vec2 v2{};
    const Common::Vec2 a2{};

    trajectory.stopped.t0 = t2;
    trajectory.stopped.t1 = std::numeric_limits<float>::max();
    trajectory.stopped.p  = p2;
    trajectory.stopped.v  = v2;
    trajectory.stopped.a  = a2;

    return trajectory;
}
} // namespace Tyr::Soccer
