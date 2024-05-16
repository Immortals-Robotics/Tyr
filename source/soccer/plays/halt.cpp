#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Halt(int robot_num)
{
    OwnRobot[robot_num].target.angle = OwnRobot[robot_num].state().angle;
    Navigate2Point(robot_num, OwnRobot[robot_num].state().position, 0);

    OwnRobot[robot_num].halt();
}

void Ai::HaltAll()
{
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        Halt(i);
    }
}
} // namespace Tyr::Soccer
