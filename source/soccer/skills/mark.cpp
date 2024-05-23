#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::mark(const int t_robot_num, const int t_opp, const float t_dist)
{
    auto oppToBall    = (m_world_state.ball.position - m_world_state.opp_robot[t_opp].position).normalized();
    auto oppToGoal    = (ownGoal() - m_world_state.opp_robot[t_opp].position).normalized();
    auto oppToGoalDis = m_world_state.opp_robot[t_opp].position.distanceTo(ownGoal());
    auto oneTouchDot  = oppToBall.dot(oppToGoal);
    if (oneTouchDot > 0 || oppToGoalDis < 2500)
    {
        mark2Goal(t_robot_num, t_opp, t_dist);
    }
    else
    {
        if (m_one_touch_detector[t_robot_num].IsArriving(20))
        {
            waitForPass(t_robot_num, false);
        }
        else
        {
            mark2Ball(t_robot_num, t_opp, t_dist);
        }
    }
}
} // namespace Tyr::Soccer
