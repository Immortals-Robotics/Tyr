#include "planner.h"

#include "../trajectory_2d_xy.h"

namespace Tyr::Soccer
{
Common::Vec2 PlannerTrajectory::plan(const Common::Vec2 init_pos, const Common::Vec2 init_vel, const Common::Vec2 target, const VelocityProfile &profile)
{
    m_profile = profile;
    m_target = target;

    const Trajectory2DXY target_trajectory = Trajectory2DXY::makeBangBangTrajectory(
        init_pos, init_vel, target, profile);
    if (!target_trajectory.hasCollision(*m_map))
    {
        return target;
    }

    std::vector<TrajectoryChained2DXY> valid_trajectories{};

    const std::vector<Common::Vec2> intermediate_points = generateIntermediateTargetsSystematic(init_pos);

    for (const Common::Vec2 &point : intermediate_points)
    {
        Common::debug().draw(Common::Circle{point, 20.0f}, Common::Color::blue(), true);

        const Trajectory2DXY trajectory = Trajectory2DXY::makeBangBangTrajectory(
            init_pos, init_vel, point, profile);

        const auto chained_trajectory = findChainedTrajectory(trajectory);
        if (chained_trajectory.has_value())
        {
            valid_trajectories.push_back(chained_trajectory.value());

            chained_trajectory.value().draw();
        }
    }

    // TODO: remove
    return init_pos;
}

std::vector<Common::Vec2> PlannerTrajectory::generateIntermediateTargetsSystematic(const Common::Vec2 t_center) const
{
    constexpr float kMinRadius       = 100.0f;
    constexpr float kRadiusStep      = 1000.0f;
    constexpr int   kRadiusStepCount = 4;

    constexpr int kAngleStepCount = 16;

    std::vector<Common::Vec2> targets{kRadiusStepCount * kAngleStepCount};

    for (int i = 0; i < kRadiusStepCount; ++i)
    {
        const float radius = kMinRadius + kRadiusStep * i;

        for (int j = 0; j < kAngleStepCount; ++j)
        {
            const Common::Angle angle = Common::Angle::fromDeg(j * 360.0f / kAngleStepCount);

            const Common::Vec2 point = t_center.circleAroundPoint(angle, radius);
            targets.push_back(point);
        }
    }

    return targets;
}
std::optional<PlannerTrajectory::TrajectoryChained2DXY> PlannerTrajectory::findChainedTrajectory(const Trajectory2DXY &trajectory) const
{
    constexpr float kTimeStep = 0.2f;
    constexpr float kLookaheadTime = 3.f;

    const float t_end = std::min(trajectory.getStartTime() + kLookaheadTime, trajectory.getEndTime());

    for (float t = trajectory.getStartTime(); t < t_end; t += kTimeStep)
    {
        const bool first_leg_valid = !trajectory.hasCollision(*m_map, t);
        if (!first_leg_valid)
        {
            break;
        }

        const Common::Vec2 pos = trajectory.getPosition(t);
        const Common::Vec2 vel = trajectory.getVelocity(t);

        const Trajectory2DXY target_trajectory = Trajectory2DXY::makeBangBangTrajectory(
        pos, vel, m_target, m_profile);
        if (!target_trajectory.hasCollision(*m_map))
        {
            return TrajectoryChained2DXY{
                trajectory,
                target_trajectory,
                t};
        }
    }

    return {};
}
} // namespace Tyr::Soccer
