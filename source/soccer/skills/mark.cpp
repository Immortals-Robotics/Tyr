#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Mark(int robot_num, int opp, float dist)
{
    auto oppToBall    = (m_world_state.ball.position - m_world_state.opp_robot[opp].position).normalized();
    auto oppToGoal    = (ownGoal() - m_world_state.opp_robot[opp].position).normalized();
    auto oppToGoalDis = m_world_state.opp_robot[opp].position.distanceTo(ownGoal());
    auto ballToOppDis = m_world_state.ball.position.distanceTo(m_world_state.opp_robot[opp].position);
    auto oneTouchDot  = oppToBall.dot(oppToGoal);
    if (oneTouchDot > 0 || oppToGoalDis < 2500)
    {
        Mark2Goal(robot_num, opp, dist);
    }
    else
    {
        if (oneTouchDetector[robot_num].IsArriving(20))
        {
            WaitForPass(robot_num, false);
        }
        else
        {
            Mark2Ball(robot_num, opp, dist);
        }
    }
}
} // namespace Tyr::Soccer
