#include "../ai.h"

namespace Tyr::Soccer
{
static float t_nml;

void Ai::penaltyUsShootout()
{
    defHi(m_def, m_rw, m_lw, nullptr, true);

    m_own_robot[m_dmf].face(oppGoal());
    navigate(m_dmf, Common::Vec2(m_side * 4000, 0), VelocityProfile::aroom(), NavigationFlagsForceBallObstacle);

    m_own_robot[m_mid1].face(oppGoal());
    navigate(m_mid1, Common::Vec2(m_side * 4000, -500), VelocityProfile::aroom(), NavigationFlagsForceBallObstacle);

    m_own_robot[m_mid2].face(oppGoal());
    navigate(m_mid2, Common::Vec2(m_side * 4000, 500), VelocityProfile::aroom(), NavigationFlagsForceBallObstacle);

    if (!m_ref_state.canKickBall())
    {
        // TODO: get behind the ball
        Common::logInfo("step0 - Waiting for permission");
    }
    else if (m_world_state.ball.position.distanceTo(oppGoal()) > 3000)
    {
        circleBall(m_attack, oppGoal().angleWith(m_world_state.ball.position), 1, 0, 0);
        Common::logInfo("step1 - Moving forward - waiting to get close to the opp goal");
    }
    else
    {
        circleBall(m_attack, oppGoal().angleWith(m_world_state.ball.position), 60, 0, 0);
        Common::logInfo("step2 - Kick in the goal!!!!");
    }
}
} // namespace Tyr::Soccer
