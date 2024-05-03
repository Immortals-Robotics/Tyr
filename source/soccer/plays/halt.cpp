#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Halt(int robot_num)
{
    // OwnRobot[robot_num].MoveByMotion(Common::Vec3(0.0f));
    OwnRobot[robot_num].target.angle = OwnRobot[robot_num].State.angle;
    Navigate2Point(robot_num, OwnRobot[robot_num].State.position, 0);
    // OwnRobot[robot_num].data[9] = 110;
    OwnRobot[robot_num].halted = true;

    navigated[robot_num] = true;
}

void Ai::HaltAll()
{
    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        Halt(i);
    }
}
} // namespace Tyr::Soccer
