#pragma once

#include "../trajectory/trajectory_piece.h"
#include "../../robot/robot.h"

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

    TrajectoryPiece ComputeSafeMotion(int robot_num, const TrajectoryPiece &cmd, const VelocityProfile &t_profile);

private:
    const ObstacleMap* m_map = nullptr;

    Common::Random m_random;

    VelocityProfile m_profile;

    const Common::WorldState *m_world;

    TrajectoryPiece cached_motions[Common::Config::Common::kMaxRobots];

    bool collisionWithOwn(const TrajectoryPiece &cmd_a,
                          const TrajectoryPiece &cmd_b) const;

    bool collisionWithOpp(const TrajectoryPiece &cmd_own,
                          const Common::RobotState &state_opp) const;

    bool RobotHasStaticCollision(const TrajectoryPiece &cmd) const;

    bool         isSafe(int robot_num, const TrajectoryPiece &cmd);
    TrajectoryPiece GetRandomAcceleration(const TrajectoryPiece &original);
    static float ComputeError(const TrajectoryPiece &target, const TrajectoryPiece &current);
};
} // namespace Tyr::Soccer
