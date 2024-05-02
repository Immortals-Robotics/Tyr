#include "ai.h"

namespace Tyr::Soccer
{
void Ai::internalFinalize()
{
    // bool saveKinoData = !OwnRobot[cmf].halted;

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
        OwnRobot[i].makeSendingDataReady();

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        m_sender->getCommand(OwnRobot[i].data);
        OwnRobot[i].halted = false;
    }
    m_sender->append_demo_data();

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            Common::worldState().last_cmds[i][j] = Common::Vec3(0.0f);
        }
    }

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        if (OwnRobot[i].State.seen_state == Common::SeenState::CompletelyOut)
            continue;
        for (int j = 0; j < 11; j++) // kheyli tof malie...
        {
            Common::worldState().last_cmds[OwnRobot[i].State.vision_id][j] = OwnRobot[i].lastCMDs[j].motion;
        }
    }
}
} // namespace Tyr::Soccer
