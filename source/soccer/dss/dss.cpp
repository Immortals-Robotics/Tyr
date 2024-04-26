#include "dss.h"

#include "../obstacle/obstacle.h"
#include "trajectory.h"

namespace Tyr::Soccer
{
Dss::Dss(const Robot *const own_robots, const Common::RobotState *const opp_robots, const float robot_r,
         const float cmd_dt, const float max_dec, const float max_dec_opp)
    : own_robots(own_robots), opp_robots(opp_robots), robot_r(robot_r), cmd_dt(cmd_dt), max_dec(max_dec),
      max_dec_opp(max_dec_opp)
{}

Common::vec2 Dss::GetAccFromMotion(const int robot_num, const Common::vec2 &motion)
{
    const Common::RobotState &state         = own_robots[robot_num].State;
    const Common::vec2        current_speed = Common::vec2(state.velocity.x, state.velocity.y);
    const Common::vec2        target_speed  = motion * 45.f;

    return (target_speed - current_speed) / cmd_dt;
}

Common::vec2 Dss::GetMotionFromAcc(const int robot_num, const Common::vec2 &acc)
{
    const Common::RobotState &state         = own_robots[robot_num].State;
    const Common::vec2        current_speed = Common::vec2(state.velocity.x, state.velocity.y);
    const Common::vec2        target_speed  = current_speed + (acc * cmd_dt);

    return target_speed / 45.f;
}

bool Dss::OwnRobotsHaveCollision(const Common::RobotState &state_a, const Common::vec2 &cmd_a,
                                 const Common::RobotState &state_b, const Common::vec2 &cmd_b) const
{
    const Trajectory traj_a = Trajectory::MakeTrajectory(state_a, cmd_a, max_dec, cmd_dt);
    const Trajectory traj_b = Trajectory::MakeTrajectory(state_b, cmd_b, max_dec, cmd_dt);

    return Parabolic::HaveOverlap(traj_a.acc, traj_b.acc, robot_r * 2.f) ||
           Parabolic::HaveOverlap(traj_a.dec, traj_b.dec, robot_r * 2.f) ||
           Parabolic::HaveOverlap(traj_a.dec, traj_b.stopped, robot_r * 2.f) ||
           Parabolic::HaveOverlap(traj_a.stopped, traj_b.dec, robot_r * 2.f);
}

bool Dss::OppRobotsHaveCollision(const Common::RobotState &state_own, const Common::vec2 &cmd_own,
                                 const Common::RobotState &state_opp) const
{
    const Trajectory traj_own = Trajectory::MakeTrajectory(state_own, cmd_own, max_dec, cmd_dt);
    const Trajectory traj_opp = Trajectory::MakeOpponentTrajectory(state_opp, max_dec);

    return Parabolic::HaveOverlap(traj_own.acc, traj_opp.dec, robot_r * 2.f) ||
           Parabolic::HaveOverlap(traj_own.dec, traj_opp.dec, robot_r * 2.f) ||
           Parabolic::HaveOverlap(traj_own.dec, traj_opp.stopped, robot_r * 2.f) ||
           Parabolic::HaveOverlap(traj_own.stopped, traj_opp.dec, robot_r * 2.f);
}

bool Dss::RobotHasStaticCollision(const Common::RobotState &state, const Common::vec2 &cmd) const
{
    const Trajectory traj = Trajectory::MakeTrajectory(state, cmd, max_dec, cmd_dt);

    return Parabolic::HasStaticOverlap(traj.acc) || Parabolic::HasStaticOverlap(traj.dec) ||
           Parabolic::HasStaticOverlap(traj.dec);
}

bool Dss::IsAccSafe(const int robot_num, const Common::vec2 &cmd)
{
    const Common::RobotState &state = own_robots[robot_num].State;

    if (RobotHasStaticCollision(state, cmd))
    {
        return false;
    }

    for (int robot_idx = 0; robot_idx < Common::Setting::kMaxOnFieldTeamRobots; ++robot_idx)
    {
        if (robot_idx == robot_num)
        {
            continue;
        }

        const Common::RobotState &other_state = own_robots[robot_idx].State;
        if (other_state.seenState == Common::CompletelyOut)
        {
            continue;
        }

        const Common::vec2 &other_cmd = computed_motions[robot_idx];

        if (OwnRobotsHaveCollision(state, cmd, other_state, other_cmd))
        {
            return false;
        }
    }

    for (int robot_idx = 0; robot_idx < Common::Setting::kMaxRobots; ++robot_idx)
    {
        const Common::RobotState &other_state = opp_robots[robot_idx];
        if (other_state.seenState == Common::CompletelyOut)
        {
            continue;
        }

        if (OppRobotsHaveCollision(state, cmd, other_state))
        {
            return false;
        }
    }

    return true;
}

Common::vec2 Dss::GetRandomAcceleration(const Common::vec2 &v, const float a_mag) const
{
    const float max_acc = 3000.f;

    const float rnd_angle     = random.get() * 2.f * 3.1415f;
    const float rnd_magnitude = random.get() * a_mag;

    return Common::vec2(cos(rnd_angle), sin(rnd_angle)) * rnd_magnitude;
}

float Dss::ComputeError(const Common::vec2 &target, const Common::vec2 &current)
{
    return Common::distance(target, current);
}

void Dss::Reset(void)
{
    for (int robot_idx = 0; robot_idx < Common::Setting::kMaxOnFieldTeamRobots; ++robot_idx)
    {
        const Common::RobotState &state = own_robots[robot_idx].State;

        if (state.seenState == Common::CompletelyOut)
        {
            computed_motions[robot_idx] = Common::vec2(0.f, 0.f);
        }
        else
        {
            const Common::vec2 v        = Common::vec2(state.velocity.x, state.velocity.y);
            const float        dec      = std::min(max_dec, Common::length(v) / cmd_dt);
            computed_motions[robot_idx] = Common::normalize(v) * (-dec);
        }
    }
}

Common::vec2 Dss::ComputeSafeMotion(const int robot_num, const Common::vec2 &motion)
{
    const Common::vec2        target_a_cmd     = GetAccFromMotion(robot_num, motion);
    const float               target_a_cmd_mag = Common::length(target_a_cmd);
    Common::vec2              a_cmd;
    const Common::RobotState &state = own_robots[robot_num].State;

    if (state.seenState == Common::CompletelyOut || IsInObstacle(state.Position))
    {
        a_cmd = target_a_cmd;
    }
    /*else if (state.vision_id != 4)
    {
        a_cmd = target_a_cmd;
    }*/
    else if (IsAccSafe(robot_num, target_a_cmd))
    {
        a_cmd = target_a_cmd;
    }
    else
    {
        const Common::vec2 v   = Common::vec2(state.velocity.x, state.velocity.y);
        const float        dec = std::min(max_dec, Common::length(v) / cmd_dt);
        a_cmd                  = Common::normalize(v) * (-dec);
        float error            = ComputeError(target_a_cmd, a_cmd);

        for (int iter_idx = 0; iter_idx < 0; ++iter_idx)
        {
            const Common::vec2 rnd_a_cmd = GetRandomAcceleration(v, target_a_cmd_mag);

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
    // if (error > 0 && state.seenState != CompletelyOut)
    {
        // std::cout << "dss changed motion: " << state.vision_id << ", error: " << error << std::endl;
    }
    computed_motions[robot_num]    = a_cmd;
    const Common::vec2 safe_motion = GetMotionFromAcc(robot_num, a_cmd);
    return safe_motion;
}
} // namespace Tyr::Soccer
