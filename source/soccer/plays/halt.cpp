#include "../ai.h"

#include "../skills/halt.h"

namespace Tyr::Soccer
{
void Ai::haltAll()
{
    for (Robot &t_robot : m_own_robot)
    {
        HaltSkill{}.execute(t_robot);
    }
}
} // namespace Tyr::Soccer
