#pragma once

#include "../../robot/robot.h"

namespace Tyr::Soccer
{
class ObstacleMap;
}
namespace Tyr::Soccer
{
class Dss
{
public:
    explicit Dss(const Common::WorldState *t_world);

    void Reset();

    void setObstacleMap(const ObstacleMap* const t_map)
    {
        m_map = t_map;
    }

    Common::Vec2 ComputeSafeMotion(int robot_num, const Common::Vec2 &motion, const VelocityProfile &t_profile);

private:
    const ObstacleMap* m_map = nullptr;

    Common::Random m_random;

    VelocityProfile m_profile;

    const Common::WorldState *m_world;

    Common::Vec2 computed_motions[Common::Config::Common::kMaxRobots];

    Common::Vec2 GetAccFromMotion(int robot_num, const Common::Vec2 &motion);
    Common::Vec2 GetMotionFromAcc(int robot_num, const Common::Vec2 &acc);

    bool collisionWithOwn(const Common::RobotState &state_a, const Common::Vec2 &cmd_a,
                          const Common::RobotState &state_b, const Common::Vec2 &cmd_b) const;

    bool collisionWithOpp(const Common::RobotState &state_own, const Common::Vec2 &cmd_own,
                          const Common::RobotState &state_opp) const;

    bool RobotHasStaticCollision(const Common::RobotState &state, const Common::Vec2 &cmd) const;

    bool         IsAccSafe(int robot_num, const Common::Vec2 &cmd);
    Common::Vec2 GetRandomAcceleration(float a_mag);
    static float ComputeError(const Common::Vec2 &target, const Common::Vec2 &current);
};
} // namespace Tyr::Soccer
