#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::stopDef()
{
    for (std::map<int *, int>::const_iterator i = m_mark_map.begin(); i != m_mark_map.end(); ++i)
    {
        m_mark_map[i->first] = -1;
    }

    if (m_own_robot[attack].state().out_for_substitute)
    {
        if (!m_own_robot[mid1].state().out_for_substitute)
        {
            std::swap(attack, mid1);
        }
        else if (!m_own_robot[mid2].state().out_for_substitute)
        {
            std::swap(attack, mid2);
        }
    }
    if (m_own_robot[mid1].state().position.y < m_own_robot[mid2].state().position.y)
    {
        std::swap(mid1, mid2);
    }

    gkHi(gk, true);
    defHi(def, rw, lw, nullptr, true);

    std::map<int, Common::Vec2> static_pos;
    static_pos[dmf]  = Common::Vec2(m_side * 4200, Common::sign(m_world_state.ball.position.y) * 1000.0f);
    static_pos[mid1] = Common::Vec2(m_side * 4500, Common::sign(-m_world_state.ball.position.y) * 3000.0f);
    static_pos[mid2] = Common::Vec2(m_side * 4200, Common::sign(-m_world_state.ball.position.y) * 1000.0f);

    m_own_robot[dmf].face(m_world_state.ball.position);
    navigate(dmf, static_pos[dmf], VelocityProfile::aroom());

    m_own_robot[mid1].face(m_world_state.ball.position);
    navigate(mid1, static_pos[mid1], VelocityProfile::aroom());

    m_own_robot[mid2].face(m_world_state.ball.position);
    navigate(mid2, static_pos[mid2], VelocityProfile::aroom());

    m_own_robot[attack].face(m_world_state.ball.position);
    navigate(attack,
             m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 580),
             VelocityProfile::aroom());
}
} // namespace Tyr::Soccer
