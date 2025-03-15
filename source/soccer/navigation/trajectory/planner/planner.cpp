#include "planner.h"

#include "../trajectory_2d_xy.h"

namespace Tyr::Soccer
{
Common::Vec2 PlannerTrajectory::nearestFree(Common::Vec2 state)
{
    const float acceptable_free_dis = 50.0f;

    // clamp the position to be inside the field
    const float margin = Common::field().boundary_width - Common::field().robot_radius;
    if (std::fabs(state.x) > Common::field().width + margin)
        state.x = Common::sign(state.x) * (Common::field().width + margin);

    if (std::fabs(state.y) > Common::field().height + margin)
        state.y = Common::sign(state.y) * (Common::field().height + margin);

    if (!m_map->inside(state))
        return state;

    Common::Vec2 ans    = state;
    float        minDis = std::numeric_limits<float>::max();

    for (int i = 0; i < 1000; i++)
    {
        Common::Vec2 newRndPoint = randomState();
        const float  tmp_d       = state.distanceSquaredTo(newRndPoint);
        if (!m_map->inside(newRndPoint) && tmp_d < minDis)
        {
            ans    = newRndPoint;
            minDis = tmp_d;
            if (minDis < acceptable_free_dis)
                break;
        }
    }

    return ans;
}

Common::Vec2 PlannerTrajectory::plan(const Common::Vec2 init_pos, const Common::Vec2 init_vel, Common::Vec2 target, const VelocityProfile &profile)
{
#if 0
    // if starting in obstacle, just get out first
    if (m_map->inside(init_pos))
    {
        target = nearestFree(init_pos);
    }
    else
#endif
        if (m_map->inside(target))
    {
        target = nearestFree(target);
    }

    m_profile = profile;
    m_target = target;

    const Trajectory2DXY target_trajectory = Trajectory2DXY::makeBangBangTrajectory(
        init_pos, init_vel, target, profile);
    if (!target_trajectory.hasCollision(*m_map).first)
    {
        return target;
    }

    const std::vector<Common::Vec2> intermediate_points = generateIntermediateTargetsSystematic(init_pos);

    TrajectoryChained2DXY best_trajectory{};
    float min_penalty = std::numeric_limits<float>::max();

    for (int i = 0; i < intermediate_points.size(); i++)
    {
        const Common::Vec2 point = intermediate_points[i];

        const Trajectory2DXY trajectory = Trajectory2DXY::makeBangBangTrajectory(
            init_pos, init_vel, point, profile);

        const TrajectoryChained2DXY chained_trajectory = findChainedTrajectory(trajectory);
        const float penalty = calculateTrajectoryPenalty(chained_trajectory);

        if (penalty < min_penalty)
        {
            min_penalty = penalty;
            best_trajectory = chained_trajectory;
        }
    }

    // check if the last intermediate point is still valid
    if (m_intermediate_target.has_value())
    {
        const Trajectory2DXY trajectory = Trajectory2DXY::makeBangBangTrajectory(
                    init_pos, init_vel, m_intermediate_target.value(), profile);

        const TrajectoryChained2DXY chained_trajectory = findChainedTrajectory(trajectory);
        const float penalty = calculateTrajectoryPenalty(chained_trajectory);

        Common::logDebug("last penalty: {}, min penalty: {}", penalty, min_penalty);

        if (penalty < min_penalty + 1.0f)
        {
            min_penalty = penalty;
            best_trajectory = chained_trajectory;
        }
    }

    best_trajectory.draw(Common::Color::blue());

    const Trajectory2DXY& second_trajectory = best_trajectory.getFirstTrajectory();
    const Common::Vec2 intermediate_point = second_trajectory.getPosition(second_trajectory.getEndTime());
    Common::debug().draw(Common::Circle{intermediate_point, 40.0f}, Common::Color::red(), true);

    m_intermediate_target = intermediate_point;

    return intermediate_point;
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

// finds a trajectory that consists of
// 1- init to time t1 somewhere on the input trajectory
// 2- trajectory from the state at t1 to target
PlannerTrajectory::TrajectoryChained2DXY PlannerTrajectory::findChainedTrajectory(const Trajectory2DXY &trajectory) const
{
    constexpr float kTimeStep      = 0.2f;

    const float t_end = std::min(trajectory.getStartTime() + kLookaheadTime, trajectory.getEndTime());

    for (float t = trajectory.getStartTime(); t < t_end; t += kTimeStep)
    {
        if (!trajectory.hasCollision(*m_map, t).first)
        {
            break;
        }

        const Common::Vec2 pos = trajectory.getPosition(t);
        const Common::Vec2 vel = trajectory.getVelocity(t);

        const Trajectory2DXY target_trajectory = Trajectory2DXY::makeBangBangTrajectory(pos, vel, m_target, m_profile);
        if (!target_trajectory.hasCollision(*m_map).first)
        {
            return TrajectoryChained2DXY{trajectory, target_trajectory, t};
        }
    }

    const Common::Vec2 pos = trajectory.getPosition(t_end);
    const Common::Vec2 vel = trajectory.getVelocity(t_end);

    const Trajectory2DXY target_trajectory = Trajectory2DXY::makeBangBangTrajectory(pos, vel, m_target, m_profile);
    return TrajectoryChained2DXY{trajectory, target_trajectory, t_end};
}

float PlannerTrajectory::calculateTrajectoryPenalty(const TrajectoryChained2DXY &trajectory) const
{
    float penalty = 0.0f;

    const auto collision = trajectory.hasCollision(*m_map, kLookaheadTime);

    penalty += trajectory.getDuration();

    if (collision.first)
    {
        penalty += 5.0f;

        penalty += std::max(0.f, kLookaheadTime - collision.second);
    }

    const auto free = trajectory.reachFree(*m_map, kLookaheadTime);
    if (free.first) // is or will eventually get free
    {
        penalty += 3.0f * free.second;
    }
    else // starts in collision and never gets free
    {
        // TODO: this is 0 in their TDP
        penalty += 3.0f * kLookaheadTime;
    }

    if (trajectory.getDuration() > kLookaheadTime)
    {
        const Common::Vec2 pos_at_lookahead = trajectory.getPosition(kLookaheadTime);
        penalty += pos_at_lookahead.distanceTo(m_target) / 1000.0f;
    }

    return penalty;
}
} // namespace Tyr::Soccer
