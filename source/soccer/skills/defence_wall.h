#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
class DefenceWallSkill final : public Skill
{
public:
    explicit DefenceWallSkill(const bool t_kick_off = false) : m_kick_off(t_kick_off)
    {}

    static const Id kId;
    Id              id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    bool m_kick_off;
};
} // namespace Tyr::Soccer
