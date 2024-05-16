#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::WaitForGool(int robot_num, bool chip)
{
    Common::Vec2 pos =
        CalculatePassPos(robot_num, Common::Vec2(-side * 3025, 0), OwnRobot[robot_num].state().position, -1600);

    OwnRobot[robot_num].face(Common::Vec2(-side * Common::field().width, 0));

    setObstacles(robot_num);
    navigate(robot_num, pos, VelocityProfile::Type::Mamooli);

    OwnRobot[robot_num].shoot(100);
    OwnRobot[robot_num].dribble(15);
}
} // namespace Tyr::Soccer
