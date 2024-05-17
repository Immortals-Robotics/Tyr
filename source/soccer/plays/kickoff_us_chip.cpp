#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoffUsChip()
{
    gkHi(m_gk);
    defHi(m_def, m_rw, m_lw, nullptr);

    m_own_robot[m_dmf].face(m_world_state.ball.position);
    navigate(m_dmf,
             m_world_state.ball.position.pointOnConnectingLine(
                 ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);

    if (m_timer.time() < 0.5)
    {
        if (m_own_robot[m_mid1].state().position.y > m_own_robot[m_mid2].state().position.y)
        {
            std::swap(m_mid1, m_mid2);
        }
    }

    m_own_robot[m_mid2].face(oppGoal());
    navigate(m_mid2, Common::Vec2(m_world_state.ball.position.x + m_side * 150, (Common::field().height - 300)),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);

    m_own_robot[m_mid1].face(oppGoal());
    navigate(m_mid1, Common::Vec2(m_world_state.ball.position.x + m_side * 150, -(Common::field().height - 300)),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);

    Common::Vec2 chip_target = Common::Vec2(-m_side * 2000, 0);
    Common::logDebug("can kick ball: {}", m_ref_state.canKickBall());
    if (m_ref_state.canKickBall())
    {
        // TODO: why isn't this circleBall?
        attacker(m_attack, chip_target.angleWith(m_world_state.ball.position), 0, 80, 0, 1);
    }
    else
    {
        circleBall(m_attack, chip_target.angleWith(m_world_state.ball.position), 0, 0, 1.0f);
    }
}
} // namespace Tyr::Soccer
