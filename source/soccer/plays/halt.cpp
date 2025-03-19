#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::halt(Robot& t_robot)
{
    if (t_robot.currentMotion().length() > 100.0f)
    {
        t_robot.fullBeak();
    }
    else
    {
        t_robot.halt();
    }
}

void Ai::haltAll()
{
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        halt(m_own_robot[i]);
    }
}
} // namespace Tyr::Soccer
