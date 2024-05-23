#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::theirPlaceBall()
{
    gkHi(m_gk);
    defHi(m_def, m_rw, m_lw, nullptr);

    if (Common::config().soccer.mark_in_stop)
    {
        markManager();

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
                else if (own == m_attack)
                {
                    m_own_robot[m_attack].face(m_world_state.ball.position);
                    navigate(m_attack,
                             m_world_state.ball.position.circleAroundPoint(
                                 m_world_state.ball.position.angleWith(ownGoal()), 650),
                             VelocityProfile::aroom());
                    m_own_robot[m_attack].shoot(0);
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

        m_own_robot[m_rw].face(m_world_state.ball.position);
        navigate(m_rw,
                 Common::Vec2(0, -100) + m_world_state.ball.position.pointOnConnectingLine(
                                             ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
                 VelocityProfile::aroom());

        m_own_robot[m_lw].face(m_world_state.ball.position);
        navigate(m_lw,
                 Common::Vec2(0, 100) + m_world_state.ball.position.pointOnConnectingLine(
                                            ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
                 VelocityProfile::aroom());

        m_own_robot[m_mid2].face(m_world_state.ball.position);
        navigate(m_mid2,
                 m_world_state.ball.position.circleAroundPoint(
                     Common::Angle::fromDeg(-20) + m_world_state.ball.position.angleWith(ownGoal()), 1090),
                 VelocityProfile::aroom());

        m_own_robot[m_mid1].face(m_world_state.ball.position);
        navigate(m_mid1,
                 m_world_state.ball.position.circleAroundPoint(
                     Common::Angle::fromDeg(20) + m_world_state.ball.position.angleWith(ownGoal()), 1090),
                 VelocityProfile::aroom());

        m_own_robot[m_attack].face(m_world_state.ball.position);
        navigate(m_attack,
                 m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 1090),
                 VelocityProfile::aroom());
    }
}
} // namespace Tyr::Soccer
