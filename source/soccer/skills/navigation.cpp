#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::navigate(int robot_num, Common::Vec2 dest, VelocityProfile profile, const NavigationFlags t_flags)
{
    if (OwnRobot[robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        return;

    setObstacles(robot_num, t_flags);

    if (m_ref_state.shouldSlowDown())
    {
        profile.max_spd = std::min(profile.max_spd, 900.0f);
    }

    if (OwnRobot[robot_num].state().position.distanceTo(dest) > 100.0f)
        Common::debug().draw(dest);

    planner[robot_num].init(OwnRobot[robot_num].state().position, dest, 90.0f);
    Common::Vec2 wayp = planner[robot_num].plan();

    planner[robot_num].draw();

    OwnRobot[robot_num].target.position = wayp;

    Common::Vec2 motion_cmd = OwnRobot[robot_num].computeMotion(profile);

    if (!(t_flags & NavigationFlagsForceNoObstacles))
        motion_cmd = dss->ComputeSafeMotion(robot_num, motion_cmd, profile);

    OwnRobot[robot_num].move(motion_cmd);
}
} // namespace Tyr::Soccer
