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

    Common::Vec2 target = m_planner[t_robot_num].plan(robot.state().position, t_dest);
    if (std::fabs(target.x) > Common::field().width + Common::field().boundary_width)
        target.x = Common::sign(target.x) * (Common::field().width + Common::field().boundary_width);

    if (std::fabs(target.y) > Common::field().height + Common::field().boundary_width)
        target.y = Common::sign(target.y) * (Common::field().height + Common::field().boundary_width);

    m_planner[t_robot_num].draw();

    robot.target.position = target;

    // TODO: use the old trajectory for now
#if 1
    const Trajectory2D trajectory = Trajectory2D::makeRobotTrajectory(robot.state().position, robot.currentMotion(), target, t_profile);
#else
    const Trajectory2DXY trajectory = Trajectory2DXY::makeBangBangTrajectory(robot.state().position, robot.currentMotion(), target, t_profile);
#endif

    trajectory.draw();

    const float dt = 1.f / Common::config().vision.vision_frame_rate;
    TrajectoryPiece2D command_piece = trajectory.getCommandPiece(dt);

    if (!(t_flags & NavigationFlagsForceNoObstacles))
    {
        m_dss->setObstacleMap(&m_obsMap[t_robot_num]);
        command_piece = m_dss->ComputeSafeMotion(t_robot_num, command_piece, t_profile);
    }

    Common::Vec2 motion_cmd = command_piece.getVelocity(dt);
    robot.move(motion_cmd);
}
} // namespace Tyr::Soccer
