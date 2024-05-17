#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::stopDef()
{
    for (std::map<int *, int>::const_iterator i = m_mark_map.begin(); i != m_mark_map.end(); ++i)
    {
        m_mark_map[i->first] = -1;
    }

    if (m_own_robot[m_attack].state().out_for_substitute)
    {
        if (!m_own_robot[m_mid1].state().out_for_substitute)
        {
            std::swap(m_attack, m_mid1);
        }
        else if (!m_own_robot[m_mid2].state().out_for_substitute)
        {
            std::swap(m_attack, m_mid2);
        }
    }
    if (m_own_robot[m_mid1].state().position.y < m_own_robot[m_mid2].state().position.y)
    {
        std::swap(m_mid1, m_mid2);
    }

    gkHi(m_gk);
    defHi(m_def, m_rw, m_lw, nullptr);

    std::map<int, Common::Vec2> static_pos;
    static_pos[m_dmf]  = Common::Vec2(m_side * 4200, Common::sign(m_world_state.ball.position.y) * 1000.0f);
    static_pos[m_mid1] = Common::Vec2(m_side * 4500, Common::sign(-m_world_state.ball.position.y) * 3000.0f);
    static_pos[m_mid2] = Common::Vec2(m_side * 4200, Common::sign(-m_world_state.ball.position.y) * 1000.0f);

    m_own_robot[m_dmf].face(m_world_state.ball.position);
    navigate(m_dmf, static_pos[m_dmf], VelocityProfile::aroom());

    m_own_robot[m_mid1].face(m_world_state.ball.position);
    navigate(m_mid1, static_pos[m_mid1], VelocityProfile::aroom());

    m_own_robot[m_mid2].face(m_world_state.ball.position);
    navigate(m_mid2, static_pos[m_mid2], VelocityProfile::aroom());

    m_own_robot[m_attack].face(m_world_state.ball.position);
    navigate(m_attack,
             m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 580),
             VelocityProfile::aroom());
}
} // namespace Tyr::Soccer
