#pragma once

#include "../robot/robot.h"

namespace Tyr::Soccer
{
class Dss
{
public:
    Dss(const Common::WorldState *t_world, float cmd_dt, float max_dec, float max_dec_opp);

    void Reset();

    Common::Vec2 ComputeSafeMotion(int robot_num, const Common::Vec2 &motion);

private:
    Common::Random m_random;

    const float cmd_dt;
    const float max_dec;
    const float max_dec_opp;

    const Common::WorldState *m_world;

    Common::Vec2 computed_motions[Common::Setting::kMaxRobots];

    Common::Vec2 GetAccFromMotion(int robot_num, const Common::Vec2 &motion);
    Common::Vec2 GetMotionFromAcc(int robot_num, const Common::Vec2 &acc);

    bool OwnRobotsHaveCollision(const Common::RobotState &state_a, const Common::Vec2 &cmd_a,
                                const Common::RobotState &state_b, const Common::Vec2 &cmd_b) const;
    bool OppRobotsHaveCollision(const Common::RobotState &state_own, const Common::Vec2 &cmd_own,
                                const Common::RobotState &state_opp) const;
    bool RobotHasStaticCollision(const Common::RobotState &state, const Common::Vec2 &cmd) const;

    bool         IsAccSafe(int robot_num, const Common::Vec2 &cmd);
    Common::Vec2 GetRandomAcceleration(const Common::Vec2 &v, float a_mag);
    static float ComputeError(const Common::Vec2 &target, const Common::Vec2 &current);
};
} // namespace Tyr::Soccer
