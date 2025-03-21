#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::penaltyUsShootout()
{
    defHi(m_own_robot[m_def1], m_own_robot[m_def2], nullptr);

    m_own_robot[m_mid5].face(Field::oppGoal());
    m_own_robot[m_mid5].navigate(Common::Vec2(m_side * 4000, 0), VelocityProfile::aroom(), NavigationFlags::ForceBallObstacle);

    m_own_robot[m_mid1].face(Field::oppGoal());
    m_own_robot[m_mid1].navigate(Common::Vec2(m_side * 4000, -500), VelocityProfile::aroom(), NavigationFlags::ForceBallObstacle);

    m_own_robot[m_mid2].face(Field::oppGoal());
    m_own_robot[m_mid2].navigate(Common::Vec2(m_side * 4000, 500), VelocityProfile::aroom(), NavigationFlags::ForceBallObstacle);

    if (!m_ref_state.canKickBall())
    {
        circleBall(m_own_robot[m_attack], Field::oppGoal().angleWith(m_world_state.ball.position), 0, 0);
        Common::logInfo("step0 - Waiting for permission");
    }
    else if (m_world_state.ball.position.distanceTo(Field::oppGoal()) > 3000)
    {
        circleBall(m_own_robot[m_attack], Field::oppGoal().angleWith(m_world_state.ball.position), 1, 0);
        Common::logInfo("step1 - Moving forward - waiting to get close to the opp goal");
    }
    else
    {
        circleBall(m_own_robot[m_attack], Field::oppGoal().angleWith(m_world_state.ball.position), 6000.0f, 0);
        Common::logInfo("step2 - Kick in the goal!!!!");
    }
}
} // namespace Tyr::Soccer
