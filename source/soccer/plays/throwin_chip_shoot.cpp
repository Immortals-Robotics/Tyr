#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::throwinChipShoot()
{
    gkHi(gk, true);
    defHi(def, rw, lw, nullptr, true);

    m_own_robot[mid2].face(m_world_state.ball.position);
    navigate(mid2, Common::Vec2(-m_side * 1500, Common::sign(m_world_state.ball.position.y) * 2500.0f),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);
    m_one_touch_type[mid2] = OneTouchType::Shirje;

    m_own_robot[mid1].face(m_world_state.ball.position);
    navigate(mid1, Common::Vec2(-m_side * 3500, Common::sign(m_world_state.ball.position.y) * 2500.0f),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);
    m_one_touch_type[mid1] = OneTouchType::Shirje;

    if (m_timer.time() > 4)
    {
        circleBall(dmf,
                   Common::Vec2(oppGoal().x, Common::sign(m_world_state.ball.position.y) * 200.0f)
                       .angleWith(m_world_state.ball.position),
                   0, 15, 1.0f);
    }
    else
    {
        circleBall(dmf, oppGoal().angleWith(m_world_state.ball.position), 0, 0, 1.0f);
    }

    m_own_robot[attack].face(oppGoal());
    if (m_random_param < 0.5)
    {
        navigate(attack,
                 m_world_state.ball.position.pointOnConnectingLine(
                     Common::Vec2(oppGoal().x, Common::sign(-m_world_state.ball.position.x) * 2000.0f), 350),
                 VelocityProfile::mamooli(), NavigationFlagsForceBallMediumObstacle);
    }
    else
    {
        navigate(attack,
                 m_world_state.ball.position.pointOnConnectingLine(
                     Common::Vec2(oppGoal().x, Common::sign(m_world_state.ball.position.x) * 2000.0f), 350),
                 VelocityProfile::mamooli(), NavigationFlagsForceBallMediumObstacle);
    }
}
} // namespace Tyr::Soccer
