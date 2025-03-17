#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::halt(const int t_robot_num)
{
    if (m_own_robot[t_robot_num].currentMotion().length() > 100.0f)
    {
        m_own_robot[t_robot_num].fullBeak();
    }
    else
    {
        m_own_robot[t_robot_num].halt();
    }
}

void Ai::haltAll()
{
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        halt(i);
    }
}
} // namespace Tyr::Soccer
