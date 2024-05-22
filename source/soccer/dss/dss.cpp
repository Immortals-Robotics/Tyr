#include "dss.h"

#include "../obstacle/map.h"
#include "trajectory.h"

namespace Tyr::Soccer
{
Dss::Dss(const Common::WorldState *const t_world, const float max_dec_opp) : max_dec_opp(max_dec_opp), m_world(t_world)
{}

Common::Vec2 Dss::GetAccFromMotion(const int robot_num, const Common::Vec2 &motion)
{
    const Common::RobotState &state = m_world->own_robot[robot_num];

    return (motion - state.velocity) * Common::config().vision.vision_frame_rate;
}

Common::Vec2 Dss::GetMotionFromAcc(const int robot_num, const Common::Vec2 &acc)
{
    const Common::RobotState &state        = m_world->own_robot[robot_num];
    const Common::Vec2        target_speed = state.velocity + (acc / Common::config().vision.vision_frame_rate);

    return target_speed;
}

bool Dss::collisionWithOwn(const Common::RobotState &state_a, const Common::Vec2 &cmd_a,
                           const Common::RobotState &state_b, const Common::Vec2 &cmd_b) const
{
    const Trajectory traj_a = Trajectory::MakeTrajectory(state_a, cmd_a, m_profile.max_dec,
                                                         1.0f / Common::config().vision.vision_frame_rate);
    const Trajectory traj_b = Trajectory::MakeTrajectory(state_b, cmd_b, m_profile.max_dec,
                                                         1.0f / Common::config().vision.vision_frame_rate);

    return Parabolic::HaveOverlap(traj_a.acc, traj_b.acc, Common::field().robot_radius * 2.f) ||
           Parabolic::HaveOverlap(traj_a.dec, traj_b.dec, Common::field().robot_radius * 2.f) ||
           Parabolic::HaveOverlap(traj_a.dec, traj_b.stopped, Common::field().robot_radius * 2.f) ||
           Parabolic::HaveOverlap(traj_a.stopped, traj_b.dec, Common::field().robot_radius * 2.f);
}

bool Dss::collisionWithOpp(const Common::RobotState &state_own, const Common::Vec2 &cmd_own,
                           const Common::RobotState &state_opp) const
{
    const Trajectory traj_own = Trajectory::MakeTrajectory(state_own, cmd_own, m_profile.max_dec,
                                                           1.0f / Common::config().vision.vision_frame_rate);
    const Trajectory traj_opp = Trajectory::MakeOpponentTrajectory(state_opp, m_profile.max_dec);

    return Parabolic::HaveOverlap(traj_own.acc, traj_opp.dec, Common::field().robot_radius * 2.f) ||
           Parabolic::HaveOverlap(traj_own.dec, traj_opp.dec, Common::field().robot_radius * 2.f) ||
           Parabolic::HaveOverlap(traj_own.dec, traj_opp.stopped, Common::field().robot_radius * 2.f) ||
           Parabolic::HaveOverlap(traj_own.stopped, traj_opp.dec, Common::field().robot_radius * 2.f);
}

bool Dss::RobotHasStaticCollision(const Common::RobotState &state, const Common::Vec2 &cmd) const
{
    const Trajectory traj =
        Trajectory::MakeTrajectory(state, cmd, m_profile.max_dec, 1.0f / Common::config().vision.vision_frame_rate);

    return Parabolic::HasStaticOverlap(traj.acc) || Parabolic::HasStaticOverlap(traj.dec) ||
           Parabolic::HasStaticOverlap(traj.dec);
}

bool Dss::IsAccSafe(const int robot_num, const Common::Vec2 &cmd)
{
    const Common::RobotState &state = m_world->own_robot[robot_num];

    if (RobotHasStaticCollision(state, cmd))
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

        const Common::Vec2 &other_cmd = computed_motions[robot_idx];

        if (collisionWithOwn(state, cmd, other_state, other_cmd))
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

        if (collisionWithOpp(state, cmd, other_state))
        {
            return false;
        }
    }

    return true;
}

Common::Vec2 Dss::GetRandomAcceleration(const float a_mag)
{
    const Common::Angle rnd_angle     = Common::Angle::fromDeg(m_random.get(0.0f, 360.0f));
    const float         rnd_magnitude = m_random.get(0.0f, a_mag);

    return rnd_angle.toUnitVec() * rnd_magnitude;
}

float Dss::ComputeError(const Common::Vec2 &target, const Common::Vec2 &current)
{
    return target.distanceTo(current);
}

void Dss::Reset()
{
    for (int robot_idx = 0; robot_idx < Common::Config::Common::kMaxRobots; ++robot_idx)
    {
        const Common::RobotState &state = m_world->own_robot[robot_idx];

        if (state.seen_state == Common::SeenState::CompletelyOut)
        {
            computed_motions[robot_idx] = Common::Vec2();
        }
        else
        {
            const float dec =
                std::min(m_profile.max_dec, state.velocity.length() * Common::config().vision.vision_frame_rate);
            computed_motions[robot_idx] = state.velocity.normalized() * (-dec);
        }
    }
}

Common::Vec2 Dss::ComputeSafeMotion(const int robot_num, const Common::Vec2 &motion, const VelocityProfile &t_profile)
{
    m_profile = t_profile;
    // TODO: in simulation setting a lower dec compared
    // to motion plan results in better avoidance.
    // Verify on the real field
    m_profile.max_dec /= 2.0f;

    const Common::Vec2        target_a_cmd     = GetAccFromMotion(robot_num, motion);
    const float               target_a_cmd_mag = target_a_cmd.length();
    Common::Vec2              a_cmd;
    const Common::RobotState &state = m_world->own_robot[robot_num];

    if (state.seen_state == Common::SeenState::CompletelyOut || g_obs_map.isInObstacle(state.position))
    {
        a_cmd = target_a_cmd;
    }
    else if (IsAccSafe(robot_num, target_a_cmd))
    {
        a_cmd = target_a_cmd;
    }
    else
    {
        const float dec =
            std::min(m_profile.max_dec, state.velocity.length() * Common::config().vision.vision_frame_rate);
        a_cmd       = state.velocity.normalized() * (-dec);
        float error = ComputeError(target_a_cmd, a_cmd);

        for (int iter_idx = 0; iter_idx < 100; ++iter_idx)
        {
            const Common::Vec2 rnd_a_cmd = GetRandomAcceleration(target_a_cmd_mag);

            const float new_error = ComputeError(target_a_cmd, rnd_a_cmd);
            if (new_error >= error)
            {
                continue;
            }

            if (!IsAccSafe(robot_num, rnd_a_cmd))
            {
                continue;
            }

            a_cmd = rnd_a_cmd;
            error = new_error;
        }
    }

    const float error = ComputeError(target_a_cmd, a_cmd);
    if (error > 0 && state.seen_state != Common::SeenState::CompletelyOut)
    {
        Common::debug().draw(Common::Circle{state.position, Common::field().robot_radius}, Common::Color::magenta(),
                             false, 30.f);
        Common::logDebug("dss changed motion: {}, error: {}", state.vision_id, error);
    }
    computed_motions[robot_num]    = a_cmd;
    const Common::Vec2 safe_motion = GetMotionFromAcc(robot_num, a_cmd);
    return safe_motion;
}
} // namespace Tyr::Soccer
