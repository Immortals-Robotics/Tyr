#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::throwinChipShoot()
{
    gkHi(m_gk);
    defHi(m_def, m_rw, m_lw, nullptr, true);

    m_own_robot[m_mid2].face(m_world_state.ball.position);
    navigate(m_mid2, Common::Vec2(-m_side * 1500, Common::sign(m_world_state.ball.position.y) * 2500.0f),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);
    m_one_touch_type[m_mid2] = OneTouchType::Shirje;

    m_own_robot[m_mid1].face(m_world_state.ball.position);
    navigate(m_mid1, Common::Vec2(-m_side * 3500, Common::sign(m_world_state.ball.position.y) * 2500.0f),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);
    m_one_touch_type[m_mid1] = OneTouchType::Shirje;

    if (m_timer.time() > 4)
    {
        circleBall(m_dmf,
                   Common::Vec2(oppGoal().x, Common::sign(m_world_state.ball.position.y) * 200.0f)
                       .angleWith(m_world_state.ball.position),
                   0, 15, 1.0f);
    }
    else
    {
        circleBall(m_dmf, oppGoal().angleWith(m_world_state.ball.position), 0, 0, 1.0f);
    }

    m_own_robot[m_attack].face(oppGoal());
    if (m_random_param < 0.5)
    {
        navigate(m_attack,
                 m_world_state.ball.position.pointOnConnectingLine(
                     Common::Vec2(oppGoal().x, Common::sign(-m_world_state.ball.position.x) * 2000.0f), 350),
                 VelocityProfile::mamooli(), NavigationFlagsForceBallMediumObstacle);
    }
    else
    {
        navigate(m_attack,
                 m_world_state.ball.position.pointOnConnectingLine(
                     Common::Vec2(oppGoal().x, Common::sign(m_world_state.ball.position.x) * 2000.0f), 350),
                 VelocityProfile::mamooli(), NavigationFlagsForceBallMediumObstacle);
    }
}
} // namespace Tyr::Soccer
