#include "../ai.h"

#include "../navigation/trajectory/trajectory_2d.h"
#include "../navigation/trajectory/trajectory_2d_xy.h"

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

#if 0
    Common::Vec2 target = m_planner_rrt[t_robot_num].plan(robot.state().position, t_dest);
    m_planner[t_robot_num].draw();
#elif 1
    Common::Vec2 target = m_planner_trajectory[t_robot_num].plan(robot.state().position, robot.currentMotion(), t_dest, t_profile);
#else
    Common::Vec2 target = t_dest;
#endif

    const float margin = Common::field().boundary_width - Common::field().robot_radius;

    if (std::fabs(target.x) > Common::field().width + margin)
        target.x = Common::sign(target.x) * (Common::field().width + margin);

    if (std::fabs(target.y) > Common::field().height + margin)
        target.y = Common::sign(target.y) * (Common::field().height + margin);

#if 0
    const Trajectory2D trajectory = Trajectory2D::makeRobotTrajectory(robot.state().position, robot.currentMotion(), target, t_profile);
#else
    const Trajectory2DXY trajectory = Trajectory2DXY::makeBangBangTrajectory(robot.state().position, robot.currentMotion(), target, t_profile);
#endif

    Common::logDebug("robot [{}] time of arrival: {}", t_robot_num, Common::global_timer().time().seconds() + trajectory.getDuration());
    trajectory.draw(Common::Color::black());

    const float dt = 1.f / Common::config().vision.vision_frame_rate;
    TrajectoryPiece2D command_piece = trajectory.getCommandPiece(dt);

    if (!(t_flags & NavigationFlagsForceNoObstacles))
    {
        m_dss->setObstacleMap(&m_obsMap[t_robot_num]);
        command_piece = m_dss->ComputeSafeMotion(t_robot_num, command_piece, t_profile);
    }

    Common::Vec2 motion_cmd = command_piece.getVelocity(dt);
    robot.move(motion_cmd);

    // this is only used for assignment, not navigation
    robot.target.position = t_dest;
}
} // namespace Tyr::Soccer
