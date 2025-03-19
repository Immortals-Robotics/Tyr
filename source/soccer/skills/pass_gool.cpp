#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::waitForGool(const int t_robot_num)
{
    Common::Vec2 pos = calculatePassPos(oppGoal(), m_own_robot[t_robot_num].state().position, -1600);

    m_own_robot[t_robot_num].face(oppGoal());

    m_own_robot[t_robot_num].navigate(pos, VelocityProfile::mamooli());

    //TODO: What is this number?
    m_own_robot[t_robot_num].shoot(6500.f);
    m_own_robot[t_robot_num].dribble(15);
}
} // namespace Tyr::Soccer
