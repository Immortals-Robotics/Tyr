#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::waitForGool(int robot_num, bool chip)
{
    Common::Vec2 pos = calculatePassPos(robot_num, oppGoal(), m_own_robot[robot_num].state().position, -1600);

    m_own_robot[robot_num].face(oppGoal());

    navigate(robot_num, pos, VelocityProfile::mamooli());

    m_own_robot[robot_num].shoot(100);
    m_own_robot[robot_num].dribble(15);
}
} // namespace Tyr::Soccer
