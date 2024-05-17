#include "../ai.h"

namespace Tyr::Soccer
{
static float t_nml;

void Ai::penaltyUsShootout()
{
    defHi(def, rw, lw, nullptr, true);

    m_own_robot[dmf].face(oppGoal());
    navigate(dmf, Common::Vec2(m_side * 4000, 0), VelocityProfile::aroom(), NavigationFlagsForceBallObstacle);

    m_own_robot[mid1].face(oppGoal());
    navigate(mid1, Common::Vec2(m_side * 4000, -500), VelocityProfile::aroom(), NavigationFlagsForceBallObstacle);

    m_own_robot[mid2].face(oppGoal());
    navigate(mid2, Common::Vec2(m_side * 4000, 500), VelocityProfile::aroom(), NavigationFlagsForceBallObstacle);

    if (!m_ref_state.canKickBall())
    {
        // TODO: get behind the ball
        Common::logInfo("step0 - Waiting for permission");
    }
    else if (m_world_state.ball.position.distanceTo(oppGoal()) > 3000)
    {
        circleBall(attack, oppGoal().angleWith(m_world_state.ball.position), 1, 0, 0);
        Common::logInfo("step1 - Moving forward - waiting to get close to the opp goal");
    }
    else
    {
        circleBall(attack, oppGoal().angleWith(m_world_state.ball.position), 60, 0, 0);
        Common::logInfo("step2 - Kick in the goal!!!!");
    }
}
} // namespace Tyr::Soccer
