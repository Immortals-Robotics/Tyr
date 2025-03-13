#include "dss.h"

#include "../obstacle/map.h"
#include "../trajectory/trajectory_2d.h"

namespace Tyr::Soccer
{
Dss::Dss(const Common::WorldState *const t_world) : m_world(t_world)
{}

bool Dss::collisionWithOwn(const TrajectoryPiece2D &cmd_a,
                           const TrajectoryPiece2D &cmd_b) const
{
    const Trajectory2D traj_a = Trajectory2D::makeDssTrajectory(cmd_a, m_profile.deceleration);
    const Trajectory2D traj_b = Trajectory2D::makeDssTrajectory(cmd_b, m_profile.deceleration);

    return traj_a.hasCollision(traj_b, Common::field().robot_radius * 2.f);
}

bool Dss::collisionWithOpp(const TrajectoryPiece2D &cmd_own,
                           const Common::RobotState &state_opp) const
{
    const Trajectory2D traj_own = Trajectory2D::makeDssTrajectory(cmd_own, m_profile.deceleration);
    const Trajectory2D traj_opp = Trajectory2D::makeStopDssTrajectory(TrajectoryPiece2D::makeOppPiece(state_opp), m_profile.deceleration);

    return traj_own.hasCollision(traj_opp, Common::field().robot_radius * 2.f);
}

bool Dss::RobotHasStaticCollision(const TrajectoryPiece2D &cmd) const
{
    const Trajectory2D traj =
        Trajectory2D::makeDssTrajectory(cmd, m_profile.deceleration);

    return traj.hasCollision(*m_map);
}

bool Dss::isSafe(const int robot_num, const TrajectoryPiece2D &cmd)
{
    if (RobotHasStaticCollision(cmd))
    {
        return false;
    }

    for (int robot_idx = 0; robot_idx < Common::Config::Common::kMaxRobots; ++robot_idx)
    {
        if (robot_idx == robot_num)
        {
            continue;
        }

        const Common::RobotState &other_state = m_world->own_robot[robot_idx];
        if (other_state.seen_state == Common::SeenState::CompletelyOut)
        {
            continue;
        }

        const TrajectoryPiece2D &other_cmd = cached_motions[robot_idx];

        if (collisionWithOwn(cmd, other_cmd))
        {
            return false;
        }
    }

    for (int robot_idx = 0; robot_idx < Common::Config::Common::kMaxRobots; ++robot_idx)
    {
        const Common::RobotState &other_state = m_world->opp_robot[robot_idx];
        if (other_state.seen_state == Common::SeenState::CompletelyOut)
        {
            continue;
        }

        if (collisionWithOpp(cmd, other_state))
        {
            return false;
        }
    }

    return true;
}

TrajectoryPiece2D Dss::GetRandomAcceleration(const TrajectoryPiece2D &original)
{
    const float a_mag = original.acc.length();
    const Common::Angle rnd_angle     = Common::Angle::fromDeg(m_random.get(0.0f, 360.0f));
    const float         rnd_magnitude = m_random.get(0.0f, a_mag);

    const Common::Vec2 acc = rnd_angle.toUnitVec() * rnd_magnitude;

    TrajectoryPiece2D random = original;
    random.acc = acc;
    return random;
}

float Dss::ComputeError(const TrajectoryPiece2D &target, const TrajectoryPiece2D &current)
{
    const Common::Vec2 a0_current = current.acc;
    const Common::Vec2 a0_target = target.acc;
    return a0_target.distanceTo(a0_current);
}

void Dss::Reset()
{
    for (int robot_idx = 0; robot_idx < Common::Config::Common::kMaxRobots; ++robot_idx)
    {
        cached_motions[robot_idx] = {};
    }
}

TrajectoryPiece2D Dss::ComputeSafeMotion(const int robot_num, const TrajectoryPiece2D &cmd, const VelocityProfile &t_profile)
{
    m_profile = t_profile;
    // TODO: in simulation setting a lower dec compared
    // to motion plan results in better avoidance.
    // Verify on the real field
    m_profile.deceleration /= 2.0f;

    TrajectoryPiece2D result;

    const Common::RobotState &state = m_world->own_robot[robot_num];

    if (state.seen_state == Common::SeenState::CompletelyOut ||
        m_map->inside(state.position) ||
        isSafe(robot_num, cmd))
    {
        result = cmd;
    }
    else
    {
        const float dt = 1.f / Common::config().vision.vision_frame_rate;
        result = Trajectory2D::makeStopDssTrajectory(cmd, m_profile.deceleration).getCommandPiece(dt);
        float error = ComputeError(cmd, result);

        for (int iter_idx = 0; iter_idx < 100; ++iter_idx)
        {
            const TrajectoryPiece2D rnd_cmd = GetRandomAcceleration(cmd);

            const float new_error = ComputeError(cmd, rnd_cmd);
            if (new_error >= error)
            {
                continue;
            }

            if (!isSafe(robot_num, rnd_cmd))
            {
                continue;
            }

            result = rnd_cmd;
            error = new_error;
        }
    }

    const float error = ComputeError(cmd, result);
    if (error > 0 && state.seen_state != Common::SeenState::CompletelyOut)
    {
        Common::debug().draw(Common::Circle{state.position, Common::field().robot_radius}, Common::Color::magenta(),
                             false, 30.f);
        Common::logDebug("dss changed motion: {}, error: {}", state.vision_id, error);
    }

    cached_motions[robot_num]    = result;
    return result;
}
} // namespace Tyr::Soccer
