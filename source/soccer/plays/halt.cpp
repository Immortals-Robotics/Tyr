#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::halt(int robot_num)
{
    m_own_robot[robot_num].halt();
}

void Ai::haltAll()
{
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        halt(i);
    }
}
} // namespace Tyr::Soccer
