#include "ai.h"

namespace Tyr::Soccer
{
void Ai::internalPrepare()
{
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        OwnRobot[i].halted = false;
    }

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            m_world_state.last_cmds[i][j] = Common::Vec3(0.0f);
        }
    }

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OwnRobot[i].state().seen_state == Common::SeenState::CompletelyOut)
            continue;
        for (int j = 0; j < 11; j++) // kheyli tof malie...
        {
            m_world_state.last_cmds[OwnRobot[i].state().vision_id][j] = OwnRobot[i].last_motions[j];
        }
    }
}
} // namespace Tyr::Soccer
