#include "../ai.h"

#include "../navigation/trajectory/trajectory_2d.h"
#include "../navigation/trajectory/trajectory_2d_xy.h"

#define USE_TRAJ 1
#define USE_RRT 0

#define USE_DSS 1

#define USE_BANG_BANG 1
#define USE_IMMORTALS 0

namespace Tyr::Soccer
{
void Ai::navigate(const int t_robot_num, const Common::Vec2 t_dest, VelocityProfile t_profile, const NavigationFlags t_flags)
{
    Robot& robot = m_own_robot[t_robot_num];

    if (robot.state().seen_state == Common::SeenState::CompletelyOut)
        return;

    setObstacles(t_robot_num, t_flags);

    if (m_ref_state.shouldSlowDown())
    {
        t_profile.speed = std::min(t_profile.speed, 900.0f);
    }

    if (robot.state().position.distanceTo(t_dest) > 100.0f)
        Common::debug().draw(t_dest);

#if USE_RRT
    Common::Vec2 target = m_planner_rrt[t_robot_num].plan(robot.state().position, t_dest);
    m_planner_rrt[t_robot_num].draw();
#elif USE_TRAJ
    Common::Vec2 target = m_planner_trajectory[t_robot_num].plan(robot.state().position, robot.currentMotion(), t_dest, t_profile);
#else
    Common::Vec2 target = t_dest;
#endif

    const float margin = Common::field().boundary_width - Common::field().robot_radius;

    if (std::fabs(target.x) > Common::field().width + margin)
        target.x = Common::sign(target.x) * (Common::field().width + margin);

    if (std::fabs(target.y) > Common::field().height + margin)
        target.y = Common::sign(target.y) * (Common::field().height + margin);

#if USE_IMMORTALS
    const Trajectory2D trajectory = Trajectory2D::makeRobotTrajectory(robot.state().position, robot.currentMotion(), target, t_profile);
#elif USE_BANG_BANG
    const Trajectory2DXY trajectory = Trajectory2DXY::makeBangBangTrajectory(robot.state().position, robot.currentMotion(), target, t_profile);
#endif

    Common::logDebug("robot [{}] time of arrival: {}", t_robot_num, Common::global_timer().time().seconds() + trajectory.getDuration());
    trajectory.draw(Common::Color::black());

    const float dt = 1.f / Common::config().vision.vision_frame_rate;
    TrajectoryPiece2D command_piece = trajectory.getCommandPiece(dt);

#if USE_DSS
    if (!(t_flags & NavigationFlagsForceNoObstacles))
    {
        m_dss->setObstacleMap(&m_obsMap[t_robot_num]);
#if USE_RRT
        command_piece = m_dss->ComputeSafeMotion(t_robot_num, command_piece, t_profile);
#elif USE_TRAJ
        // TODO: this just sets the profile
        m_dss->ComputeSafeMotion(t_robot_num, command_piece, t_profile);
        if (!m_obsMap[t_robot_num].inside(robot.state().position) &&
            !m_dss->isSafe(t_robot_num, command_piece))
        {
            Common::debug().draw(Common::Circle{robot.state().position, Common::field().robot_radius}, Common::Color::magenta(),
                             false, 30.f);

            robot.fullBeak(2.f);
        }
#endif
    }
#endif

    if (!robot.navigated())
    {
        Common::Vec2 motion_cmd = command_piece.getVelocity(dt);
        robot.move(motion_cmd);
    }

    // this is only used for assignment, not navigation
    robot.target.position = t_dest;
}
} // namespace Tyr::Soccer
