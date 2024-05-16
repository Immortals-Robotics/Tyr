#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::WaitForGool(int robot_num, bool chip)
{
    Common::Vec2 pos = CalculatePassPos(robot_num, oppGoal(), OwnRobot[robot_num].state().position, -1600);

    OwnRobot[robot_num].face(oppGoal());

    navigate(robot_num, pos, VelocityProfile::mamooli());

    OwnRobot[robot_num].shoot(100);
    OwnRobot[robot_num].dribble(15);
}
} // namespace Tyr::Soccer
