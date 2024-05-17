#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::stop()
{
    if (Common::setting().mark_in_stop)
    {
        for (std::map<int *, int>::const_iterator i = m_mark_map.begin(); i != m_mark_map.end(); ++i)
        {
            m_mark_map[i->first] = -1;
        }
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
    if (m_side * m_own_robot[m_mid1].state().position.y < m_side * m_own_robot[m_mid2].state().position.y) // Nice :D
    {
        std::swap(m_mid1, m_mid2);
    }

    gkHi(m_gk, true);
    defHi(m_def, m_rw, m_lw, nullptr, true);

    if (Common::setting().mark_in_stop)
    {
        markManager(false);

        for (std::map<int *, int>::const_iterator i = m_mark_map.begin(); i != m_mark_map.end(); ++i)
        {
            int opp = i->second;
            int own = *i->first;

            if (opp == -1)
            {
                if (own == m_dmf)
                {
                    m_own_robot[m_dmf].face(m_world_state.ball.position);
                    navigate(m_dmf,
                             m_world_state.ball.position.pointOnConnectingLine(
                                 ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
                             VelocityProfile::aroom());
                    m_own_robot[m_dmf].shoot(0);
                }
                else if (own == m_mid2)
                {
                    m_own_robot[m_mid2].face(m_world_state.ball.position);
                    navigate(m_mid2,
                             m_world_state.ball.position.circleAroundPoint(
                                 Common::Angle::fromDeg(-20) + m_world_state.ball.position.angleWith(ownGoal()), 650),
                             VelocityProfile::aroom());
                    m_own_robot[m_mid2].shoot(0);
                }
                else if (own == m_mid1)
                {
                    m_own_robot[m_mid1].face(m_world_state.ball.position);
                    navigate(m_mid1,
                             m_world_state.ball.position.circleAroundPoint(
                                 Common::Angle::fromDeg(20.0f) + m_world_state.ball.position.angleWith(ownGoal()), 650),
                             VelocityProfile::aroom());
                    m_own_robot[m_mid1].shoot(0);
                }
            }
            else
            {
                mark(own, opp, 500);
            }
        }
    }
    else
    {
        m_own_robot[m_dmf].face(m_world_state.ball.position);
        navigate(m_dmf,
                 m_world_state.ball.position.pointOnConnectingLine(
                     ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
                 VelocityProfile::aroom());
        m_own_robot[m_dmf].shoot(0);

        m_own_robot[m_mid2].face(m_world_state.ball.position);
        navigate(m_mid2,
                 m_world_state.ball.position.circleAroundPoint(
                     Common::Angle::fromDeg(-20.0f) + m_world_state.ball.position.angleWith(ownGoal()), 650),
                 VelocityProfile::aroom());
        m_own_robot[m_mid2].shoot(0);

        m_own_robot[m_mid1].face(m_world_state.ball.position);
        navigate(m_mid1,
                 m_world_state.ball.position.circleAroundPoint(
                     Common::Angle::fromDeg(20.0f) + m_world_state.ball.position.angleWith(ownGoal()), 650),
                 VelocityProfile::aroom());
        m_own_robot[m_mid1].shoot(0);
    }

    m_own_robot[m_attack].face(m_world_state.ball.position);
    navigate(m_attack,
             m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 650),
             VelocityProfile::aroom());
    m_own_robot[m_attack].shoot(0);
}
} // namespace Tyr::Soccer
