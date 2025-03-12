#pragma once

#include "../../robot/robot.h"
#include "../trajectory/piece/piece_2d.h"

namespace Tyr::Soccer
{
class ObstacleMap;

class Dss
{
public:
    explicit Dss(const Common::WorldState *t_world);

    void Reset();

    void setObstacleMap(const ObstacleMap* const t_map)
    {
        m_map = t_map;
    }

    TrajectoryPiece2D ComputeSafeMotion(int robot_num, const TrajectoryPiece2D &cmd, const VelocityProfile &t_profile);

private:
    const ObstacleMap* m_map = nullptr;

    Common::Random m_random;

    VelocityProfile m_profile;

    const Common::WorldState *m_world;

    TrajectoryPiece2D cached_motions[Common::Config::Common::kMaxRobots];

    bool collisionWithOwn(const TrajectoryPiece2D &cmd_a,
                          const TrajectoryPiece2D &cmd_b) const;

    bool collisionWithOpp(const TrajectoryPiece2D &cmd_own,
                          const Common::RobotState &state_opp) const;

    bool RobotHasStaticCollision(const TrajectoryPiece2D &cmd) const;

    bool         isSafe(int robot_num, const TrajectoryPiece2D &cmd);
    TrajectoryPiece2D GetRandomAcceleration(const TrajectoryPiece2D &original);
    static float ComputeError(const TrajectoryPiece2D &target, const TrajectoryPiece2D &current);
};
} // namespace Tyr::Soccer
