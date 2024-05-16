#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::throwin_chip_shoot()
{
    GKHi(gk, true);
    DefHi(def, rw, lw, nullptr, true);

    setObstacles(mid2, true);
    OwnRobot[mid2].face(m_world_state.ball.position);
    navigate(mid2, Common::Vec2(-side * 1500, Common::sign(m_world_state.ball.position.y) * 2500.0f),
             VelocityProfile::Type::Mamooli);
    oneTouchType[mid2] = shirje;

    setObstacles(mid1, true);
    OwnRobot[mid1].face(m_world_state.ball.position);
    navigate(mid1, Common::Vec2(-side * 3500, Common::sign(m_world_state.ball.position.y) * 2500.0f),
             VelocityProfile::Type::Mamooli);
    oneTouchType[mid1] = shirje;

    if (timer.time() > 4)
    {
        // tech_circle(dmf,Common::Vec2::angleWith ( Common::Vec2 ( -side*2995 , 0 ) ,
        // m_world_state.ball.position ) ,0,30,0,1);
        circle_ball(dmf,
                    Common::Vec2(-side * Common::field().width, Common::sign(m_world_state.ball.position.y) * 200.0f)
                        .angleWith(m_world_state.ball.position),
                    0, 15, 1.0f);
    }
    else
    {
        // tech_circle(dmf,Common::Vec2::angleWith ( Common::Vec2 ( -side*2995 , 0 ) ,
        // m_world_state.ball.position ) ,0,0,0,1);
        circle_ball(dmf, Common::Vec2(-side * Common::field().width, 0).angleWith(m_world_state.ball.position), 0, 0,
                    1.0f);
    }

    OwnRobot[attack].face(Common::Vec2(-side * Common::field().width, 0));
    setObstacles(attack);
    g_obs_map.addCircle({m_world_state.ball.position, 320.0f});
    if (randomParam < 0.0)
        navigate(attack, m_world_state.ball.position.pointOnConnectingLine(
                             Common::Vec2(-side * Common::field().width, 0), 350));
    else if (randomParam < 0.5)
        navigate(attack, m_world_state.ball.position.pointOnConnectingLine(
                             Common::Vec2(-side * Common::field().width,
                                          Common::sign(-m_world_state.ball.position.x) * 2000.0f),
                             350));
    else
        navigate(attack,
                 m_world_state.ball.position.pointOnConnectingLine(
                     Common::Vec2(-side * Common::field().width, Common::sign(m_world_state.ball.position.x) * 2000.0f),
                     350));
}
} // namespace Tyr::Soccer