#include "halt.h"

#include "../robot/robot.h"

namespace Tyr::Soccer
{
const Skill::Id HaltSkill::kId = &HaltSkill::kId;

void HaltSkill::execute(Robot &t_robot)
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
} // namespace Tyr::Soccer
