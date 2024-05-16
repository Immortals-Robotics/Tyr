#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoff_us_chip()
{
    GKHi(gk, true);
    DefHi(def, rw, lw, nullptr, true);

    setObstacles(dmf, true);
    OwnRobot[dmf].face(m_world_state.ball.position);
    navigate(dmf,
             m_world_state.ball.position.pointOnConnectingLine(
                 ownGoal(),
                 m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
             VelocityProfile::mamooli());

    if (timer.time() < 0.5)
    {
        if (OwnRobot[mid1].state().position.y > OwnRobot[mid2].state().position.y)
        {
            std::swap(mid1, mid2);
        }
    }

    OwnRobot[mid2].face(oppGoal());
    setObstacles(mid2, true);
    navigate(mid2, Common::Vec2(m_world_state.ball.position.x + side * 150, (Common::field().height - 300)));
    OwnRobot[mid1].face(oppGoal());
    setObstacles(mid1, true);
    navigate(mid1, Common::Vec2(m_world_state.ball.position.x + side * 150, -(Common::field().height - 300)));
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
