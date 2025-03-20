#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
class HaltSkill final : public Skill
{
public:
    explicit HaltSkill()
    {}

    static const Id kId;
    Id              id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;
};
} // namespace Tyr::Soccer
