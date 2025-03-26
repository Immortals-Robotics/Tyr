#include "robot.h"

#include "../navigation/trajectory/trajectory_2d.h"

#define NAVIGATION_JOB 1

namespace Tyr::Soccer
{
void Robot::navigate(const Common::Vec2 t_dest, VelocityProfile t_profile, const NavigationFlags t_flags)
{
    if (state().seen_state == Common::SeenState::CompletelyOut)
    {
        return;
    }

    if (m_navigated)
    {
        Common::logWarning("Robot {} is navigated more than once", state().vision_id);
        waitForNavigationJob();
    }

    m_navigated = true;
    // this is only used for assignment, not navigation
    target.position = t_dest;

#if NAVIGATION_JOB
    m_navigation_future = std::async(std::launch::async, &Robot::navigateJob, this, t_dest, t_profile, t_flags);
#else
    navigateJob(t_dest, t_profile, t_flags);
#endif
}

void Robot::waitForNavigationJob()
{
#if NAVIGATION_JOB
    if (m_navigation_future.valid())
    {
        m_navigation_future.get();
    }
#endif
}

float Robot::calculateReachTime(const Common::Vec2 t_dest, const VelocityProfile t_profile) const
{
    const Trajectory2D trajectory =
        Trajectory2D::makeBangBangTrajectory(state().position, currentMotion(), t_dest, t_profile);
    return trajectory.getDuration();
}

void Robot::navigateJob(Common::Vec2 t_dest, VelocityProfile t_profile, NavigationFlags t_flags)
{
    setObstacles(t_flags);

    if (State::ref().shouldSlowDown())
    {
        t_profile.speed = std::min(t_profile.speed, 900.0f);
    }

    if (state().position.distanceTo(t_dest) > 100.0f)
        Common::debug().draw(t_dest);

    m_planner.setObstacleMap(&m_obs_map);
    Common::Vec2 dest = m_planner.plan(state().position, currentMotion(), t_dest, t_profile);

    const float margin = Common::field().boundary_width - Common::field().robot_radius;

    if (std::fabs(dest.x) > Common::field().width + margin)
        dest.x = Common::sign(dest.x) * (Common::field().width + margin);

    if (std::fabs(dest.y) > Common::field().height + margin)
        dest.y = Common::sign(dest.y) * (Common::field().height + margin);

    Trajectory2D trajectory = Trajectory2D::makeBangBangTrajectory(state().position, currentMotion(), dest, t_profile);

    if (!(t_flags & NavigationFlags::NoBreak))
    {
        VelocityProfile stop_profile = VelocityProfile::mamooli();
        stop_profile.acceleration *= 2.0f;

        // below this speed we consider the collision "ok" even if it happens
        // we only break when above this speed
        // and slow down only to below this speed
        static constexpr float kBreakSpeedThreshold = 500.0f;

        const float extra_speed = std::max(0.0f, currentMotion().length() - kBreakSpeedThreshold);

        const float collision_lookahead = 3.0f * extra_speed / stop_profile.acceleration;
        const bool  collision_imminent  = m_obs_map.hasCollision(trajectory, collision_lookahead, 0.1f,
                                                               Physicality::Physical).first;

        if (collision_imminent)
        {
            Common::debug().draw(Common::Circle{state().position, Common::field().robot_radius * 1.5f},
                                 Common::Color::red(), false, 50.f);

            trajectory = Trajectory2D::makeFullStopTrajectory(state().position, currentMotion(), stop_profile);
        }
    }

    move(trajectory);
}
} // namespace Tyr::Soccer
