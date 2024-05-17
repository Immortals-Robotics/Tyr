#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoffUsChip()
{
    gkHi(gk, true);
    defHi(def, rw, lw, nullptr, true);

    m_own_robot[dmf].face(m_world_state.ball.position);
    navigate(dmf,
             m_world_state.ball.position.pointOnConnectingLine(
                 ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);

    if (m_timer.time() < 0.5)
    {
        if (m_own_robot[mid1].state().position.y > m_own_robot[mid2].state().position.y)
        {
            std::swap(mid1, mid2);
        }
    }

    m_own_robot[mid2].face(oppGoal());
    navigate(mid2, Common::Vec2(m_world_state.ball.position.x + m_side * 150, (Common::field().height - 300)),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);

    m_own_robot[mid1].face(oppGoal());
    navigate(mid1, Common::Vec2(m_world_state.ball.position.x + m_side * 150, -(Common::field().height - 300)),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);

    Common::Vec2 chip_target = Common::Vec2(-m_side * 2000, 0);
    Common::logDebug("can kick ball: {}", m_ref_state.canKickBall());
    if (m_ref_state.canKickBall())
    {
        attacker(attack, chip_target.angleWith(m_world_state.ball.position), 0, 80, 1, 0, 1);
    }
    else
    {
        circleBall(attack, chip_target.angleWith(m_world_state.ball.position), 0, 0, 1.0f);
    }
}
} // namespace Tyr::Soccer
