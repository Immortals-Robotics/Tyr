#pragma once

#include "../../../robot/velocity_profile.h"
#include "../../obstacle/map.h"
#include "../trajectory_2d_xy.h"
#include "../trajectory_chained.h"

namespace Tyr::Soccer
{
// trajectory-based path planner as described in Tigers' 2019 TDP
class PlannerTrajectory
{
public:
     PlannerTrajectory() = default;
    ~PlannerTrajectory() = default;

    Common::Vec2 plan(Common::Vec2 init_pos, Common::Vec2 init_vel, Common::Vec2 target, const VelocityProfile &profile);

    void setObstacleMap(const ObstacleMap* const t_map)
    {
        m_map = t_map;
    }

private:
    const ObstacleMap* m_map = nullptr;

    std::optional<Common::Vec2> m_intermediate_target;

    VelocityProfile m_profile;
    Common::Vec2 m_target;

    static constexpr float kLookaheadTime = 3.f;

    using TrajectoryChained2DXY = TrajectoryChained<Trajectory2DXY>;

    std::vector<Common::Vec2> generateIntermediateTargetsSystematic(Common::Vec2 t_center) const;

    TrajectoryChained2DXY findChainedTrajectory(const Trajectory2DXY& trajectory) const;

    float calculateTrajectoryPenalty(const TrajectoryChained2DXY& trajectory) const;

};
} // namespace Tyr::Soccer
