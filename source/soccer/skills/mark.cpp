#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Mark(int robot_num, int opp, float dist)
{
    auto oppToBall    = (ball.Position - OppRobot[opp].Position).normalized();
    auto oppToGoal    = (Common::Vec2(side * field_width, 0) - OppRobot[opp].Position).normalized();
    auto oppToGoalDis = OppRobot[opp].Position.distanceTo(Common::Vec2(side * field_width, 0));
    auto ballToOppDis = ball.Position.distanceTo(OppRobot[opp].Position);
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
