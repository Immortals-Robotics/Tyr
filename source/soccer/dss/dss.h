#pragma once

#include "../robot/robot.h"

namespace Tyr::Soccer
{
class Dss
{
private:
    Common::Random random;

    const float robot_r;
    const float cmd_dt;
    const float max_dec;
    const float max_dec_opp;

    const Robot *const              own_robots;
    const Common::RobotState *const opp_robots;

    Common::vec2 computed_motions[Common::Setting::kMaxOnFieldTeamRobots];

    Common::vec2 GetAccFromMotion(const int robot_num, const Common::vec2 &motion);
    Common::vec2 GetMotionFromAcc(const int robot_num, const Common::vec2 &acc);

    bool OwnRobotsHaveCollision(const Common::RobotState &state_a, const Common::vec2 &cmd_a,
                                const Common::RobotState &state_b, const Common::vec2 &cmd_b) const;
    bool OppRobotsHaveCollision(const Common::RobotState &state_own, const Common::vec2 &cmd_own,
                                const Common::RobotState &state_opp) const;
    bool RobotHasStaticCollision(const Common::RobotState &state, const Common::vec2 &cmd) const;

    bool         IsAccSafe(const int robot_num, const Common::vec2 &cmd);
    Common::vec2 GetRandomAcceleration(const Common::vec2 &v, const float a_mag) const;
    static float ComputeError(const Common::vec2 &target, const Common::vec2 &current);

public:
    Dss(const Robot *const own_robots, const Common::RobotState *const opp_robots, const float robot_r,
        const float cmd_dt, const float max_dec, const float max_dec_opp);

    void Reset(void);

    Common::vec2 ComputeSafeMotion(const int robot_num, const Common::vec2 &motion);
};
} // namespace Tyr::Soccer
