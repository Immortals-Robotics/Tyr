#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::throwin_chip_shoot()
{
    GKHi(gk, true);
    DefHi(def, rw, lw, nullptr, true);

    OwnRobot[mid2].face(m_world_state.ball.position);
    navigate(mid2, Common::Vec2(-side * 1500, Common::sign(m_world_state.ball.position.y) * 2500.0f),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);
    oneTouchType[mid2] = shirje;

    OwnRobot[mid1].face(m_world_state.ball.position);
    navigate(mid1, Common::Vec2(-side * 3500, Common::sign(m_world_state.ball.position.y) * 2500.0f),
             VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);
    oneTouchType[mid1] = shirje;

    if (timer.time() > 4)
    {
        circle_ball(dmf,
                    Common::Vec2(oppGoal().x, Common::sign(m_world_state.ball.position.y) * 200.0f)
                        .angleWith(m_world_state.ball.position),
                    0, 15, 1.0f);
    }
    else
    {
        circle_ball(dmf, oppGoal().angleWith(m_world_state.ball.position), 0, 0, 1.0f);
    }

    OwnRobot[attack].face(oppGoal());
    if (randomParam < 0.5)
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