#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::navigate(const int t_robot_num, const Common::Vec2 t_dest, VelocityProfile t_profile, const NavigationFlags t_flags)
{
    if (m_own_robot[t_robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        return;

    setObstacles(t_robot_num, t_flags);

    if (m_ref_state.shouldSlowDown())
    {
        t_profile.speed = std::min(t_profile.speed, 900.0f);
    }

    if (m_own_robot[t_robot_num].state().position.distanceTo(t_dest) > 100.0f)
        Common::debug().draw(t_dest);

    Common::Vec2 wayp = m_planner[t_robot_num].plan(m_own_robot[t_robot_num].state().position, t_dest);

    m_planner[t_robot_num].draw();

    m_own_robot[t_robot_num].target.position = wayp;

    Common::Vec2 motion_cmd = m_own_robot[t_robot_num].computeMotion(t_profile);

    if (!(t_flags & NavigationFlagsForceNoObstacles))
    {
        m_dss->setObstacleMap(&m_obsMap[t_robot_num]);
        motion_cmd = m_dss->ComputeSafeMotion(t_robot_num, motion_cmd, t_profile);
    }

    m_own_robot[t_robot_num].move(motion_cmd);
}
} // namespace Tyr::Soccer
