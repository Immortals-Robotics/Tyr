#pragma once

#include "../robot/robot.h"

namespace Tyr::Soccer
{
class Dss
{
private:
    Common::Random m_random;

    const float robot_r;
    const float cmd_dt;
    const float max_dec;
    const float max_dec_opp;

    const Robot *const              own_robots;
    const Common::RobotState *const opp_robots;

    Common::Vec2 computed_motions[Common::Setting::kMaxOnFieldTeamRobots];

    Common::Vec2 GetAccFromMotion(const int robot_num, const Common::Vec2 &motion);
    Common::Vec2 GetMotionFromAcc(const int robot_num, const Common::Vec2 &acc);

    bool OwnRobotsHaveCollision(const Common::RobotState &state_a, const Common::Vec2 &cmd_a,
                                const Common::RobotState &state_b, const Common::Vec2 &cmd_b) const;
    bool OppRobotsHaveCollision(const Common::RobotState &state_own, const Common::Vec2 &cmd_own,
                                const Common::RobotState &state_opp) const;
    bool RobotHasStaticCollision(const Common::RobotState &state, const Common::Vec2 &cmd) const;

    bool         IsAccSafe(const int robot_num, const Common::Vec2 &cmd);
    Common::Vec2 GetRandomAcceleration(const Common::Vec2 &v, const float a_mag);
    static float ComputeError(const Common::Vec2 &target, const Common::Vec2 &current);

public:
    Dss(const Robot *const own_robots, const Common::RobotState *const opp_robots, const float robot_r,
        const float cmd_dt, const float max_dec, const float max_dec_opp);

    void Reset();

    Common::Vec2 ComputeSafeMotion(const int robot_num, const Common::Vec2 &motion);
};
} // namespace Tyr::Soccer
