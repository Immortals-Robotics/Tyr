#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::waitForGool(int t_robot_num)
{
    Common::Vec2 pos = calculatePassPos(oppGoal(), m_own_robot[t_robot_num].state().position, -1600);

    m_own_robot[t_robot_num].face(oppGoal());

    navigate(t_robot_num, pos, VelocityProfile::mamooli());

    m_own_robot[t_robot_num].shoot(100);
    m_own_robot[t_robot_num].dribble(15);
}
} // namespace Tyr::Soccer
