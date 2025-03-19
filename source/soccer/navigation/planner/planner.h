#pragma once

#include "../../robot/velocity_profile.h"
#include "../obstacle/map.h"
#include "../trajectory/trajectory_2d.h"
#include "../trajectory/trajectory_chained.h"

namespace Tyr::Soccer
{
// trajectory-based path planner as described in Tigers' 2019 TDP
class Planner
{
public:
     Planner() = default;
    ~Planner() = default;

    Common::Vec2 plan(Common::Vec2 init_pos, Common::Vec2 init_vel, Common::Vec2 target, const VelocityProfile &profile);

    void setObstacleMap(const ObstacleMap* const t_map)
    {
        m_map = t_map;
    }

private:
    Common::Random m_random;

    Common::Vec2 randomState()
    {
        const float margin = Common::field().boundary_width - Common::field().robot_radius;
        return Common::Vec2((m_random.get(-1.0f, 1.0f) * (Common::field().width + margin)),
                            (m_random.get(-1.0f, 1.0f) * (Common::field().height + margin)));
    }

    Common::Vec2 nearestFree(Common::Vec2 state);

    const ObstacleMap* m_map = nullptr;

    std::optional<Common::Vec2> m_intermediate_target;

    VelocityProfile m_profile;
    Common::Vec2 m_target;

    static constexpr float kLookaheadTime = 3.f;

    using TrajectoryChained2DXY = TrajectoryChained<Common::Vec2, Trajectory2D>;

    std::vector<Common::Vec2> generateIntermediateTargetsSystematic(Common::Vec2 t_center) const;

    TrajectoryChained2DXY findChainedTrajectory(const Trajectory2D& trajectory) const;

    float calculateTrajectoryPenalty(const TrajectoryChained2DXY& trajectory) const;

};
} // namespace Tyr::Soccer
