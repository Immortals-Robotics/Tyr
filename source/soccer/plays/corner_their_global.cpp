#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::cornerTheirGlobal()
{
    if (m_side * m_world_state.ball.position.x > Common::field().width - 1000)
    {
        m_own_robot[m_gk].target.angle = Common::Angle::fromDeg((1 + m_side) * 90.0f);
        navigate(m_gk, Common::Vec2(m_side * (Common::field().width - 100), 0), VelocityProfile::mamooli());

        defHi(m_def, m_rw, m_lw, nullptr, true);
    }
    else
    {
        gkHi(m_gk);
        defHi(m_def, m_rw, m_lw, nullptr, true);
    }

    m_is_defending = true;
    defenceWall(m_attack, false);

    std::map<int, Common::Vec2> static_pos;
    static_pos[m_dmf]  = Common::Vec2(m_side * 3500, -Common::sign(m_world_state.ball.position.y) * 1100.0f);
    static_pos[m_mid1] = Common::Vec2(m_side * 3200, 600);
    static_pos[m_mid2] = Common::Vec2(m_side * 3200, 0);

    markManager(true);

    for (std::map<int *, int>::const_iterator i = m_mark_map.begin(); i != m_mark_map.end(); ++i)
    {
        int opp = i->second;
        int own = *i->first;

        if (opp == -1)
        {
            if (static_pos.find(own) != static_pos.end())
            {
                m_own_robot[own].face(oppGoal());
                navigate(own, static_pos[own], VelocityProfile::mamooli());
            }
        }
        else
        {
            mark(own, opp, 500);
        }
    }
}
} // namespace Tyr::Soccer
