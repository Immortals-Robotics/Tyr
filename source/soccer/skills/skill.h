#pragma once

#include "../robot/robot.h"

namespace Tyr::Soccer::Skill
{
class ISkill
{
public:
    virtual void execute(Robot *t_robot) = 0;
};

} // namespace Tyr::Soccer