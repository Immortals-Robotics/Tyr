#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::navigate(int robot_num, Common::Vec2 dest, VelocityProfile profile, const NavigationFlags t_flags)
{
    if (m_own_robot[robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        return;

    setObstacles(robot_num, t_flags);

    if (m_ref_state.shouldSlowDown())
    {
        profile.max_spd = std::min(profile.max_spd, 900.0f);
    }

    if (m_own_robot[robot_num].state().position.distanceTo(dest) > 100.0f)
        Common::debug().draw(dest);

    m_planner[robot_num].init(m_own_robot[robot_num].state().position, dest, 90.0f);
    Common::Vec2 wayp = m_planner[robot_num].plan();

    m_planner[robot_num].draw();

    m_own_robot[robot_num].target.position = wayp;

    Common::Vec2 motion_cmd = m_own_robot[robot_num].computeMotion(profile);

    if (!(t_flags & NavigationFlagsForceNoObstacles))
        motion_cmd = m_dss->ComputeSafeMotion(robot_num, motion_cmd, profile);

    m_own_robot[robot_num].move(motion_cmd);
}
} // namespace Tyr::Soccer
