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

    trajectory.acc = TrajectoryPiece{a0, v0, p0, t0, t1};

    const Common::Vec2 p1 = trajectory.acc.getPosition(t1);
    const Common::Vec2 v1 = trajectory.acc.getVelocity(t1);
    const Common::Vec2 a1 = v1.normalized() * (-a_dec);

    const float dec_dt = v1.length() / a_dec;
    const float t2     = t1 + dec_dt;

    trajectory.dec = TrajectoryPiece{a1, v1, p1, t1, t2};

    const Common::Vec2 p2 = trajectory.dec.getPosition(t2);

    trajectory.stopped = TrajectoryPiece{{}, {}, p2, t2, std::numeric_limits<float>::max()};

    return trajectory;
}

Trajectory Trajectory::MakeOpponentTrajectory(const Common::RobotState &state, const float a_dec)
{
    Trajectory trajectory;

    // TODO: why not 0?
    const float t0 = -1.f;

    const Common::Vec2 p0 = state.position;
    const Common::Vec2 v0 = state.velocity;

    const float t1 = 0.f;

    trajectory.acc = TrajectoryPiece{{}, v0, p0, t0, t1};

    const Common::Vec2 p1 = p0;
    const Common::Vec2 v1 = v0;
    const Common::Vec2 a1 = v1.normalized() * (-a_dec);

    const float dec_dt = v1.length() / a_dec;
    const float t2     = t1 + dec_dt;

    trajectory.dec = TrajectoryPiece{a1, v1, p1, t1, t2};

    const Common::Vec2 p2 = trajectory.dec.getPosition(t2);

    trajectory.stopped = TrajectoryPiece{{}, {}, p2, t2, std::numeric_limits<float>::max()};

    return trajectory;
}
} // namespace Tyr::Soccer
