#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoff_us_chip()
{
    GKHi(gk, true);
    DefBy3(def, rw, lw, nullptr, true);

    ERRTSetObstacles(dmf, true, true);
    OwnRobot[dmf].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        dmf,
        m_world_state.ball.position.pointOnConnectingLine(
            Common::Vec2(side * Common::field().width, 0),
            m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        40, VelocityProfile::Type::Mamooli);

    if (timer.time() < 0.5)
    {
        if (OwnRobot[mid1].state().position.y > OwnRobot[mid2].state().position.y)
        {
            std::swap(mid1, mid2);
        }
    }

    OwnRobot[mid2].face(Common::Vec2(-side * Common::field().width, 0));
    ERRTSetObstacles(mid2, true, true);
    ERRTNavigate2Point(mid2, Common::Vec2(m_world_state.ball.position.x + side * 150, (Common::field().height - 300)));
    OwnRobot[mid1].face(Common::Vec2(-side * Common::field().width, 0));
    ERRTSetObstacles(mid1, true, true);
    ERRTNavigate2Point(mid1, Common::Vec2(m_world_state.ball.position.x + side * 150, -(Common::field().height - 300)));
    Common::Vec2 chip_target = Common::Vec2(-side * 2000, 0);

    Common::logDebug("can kick ball: {}", m_ref_state.canKickBall());
    if (m_ref_state.canKickBall())
    {
        tech_circle(attack, chip_target.angleWith(m_world_state.ball.position), 0, 80, 0, 1, 0, 1);
    }
    else
    {
        circle_ball(attack, chip_target.angleWith(m_world_state.ball.position), 0, 0, 1.0f);
    }
}
} // namespace Tyr::Soccer
