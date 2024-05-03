#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Mark(int robot_num, int opp, float dist)
{
    auto oppToBall    = (Common::worldState().ball.position - Common::worldState().opp_robot[opp].position).normalized();
    auto oppToGoal    = (Common::Vec2(side * Common::worldState().field.width, 0) - Common::worldState().opp_robot[opp].position).normalized();
    auto oppToGoalDis = Common::worldState().opp_robot[opp].position.distanceTo(Common::Vec2(side * Common::worldState().field.width, 0));
    auto ballToOppDis = Common::worldState().ball.position.distanceTo(Common::worldState().opp_robot[opp].position);
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
