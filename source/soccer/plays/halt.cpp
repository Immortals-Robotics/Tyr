#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Halt(int robot_num)
{
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
