#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
class DribbleToDirectionSkill final : public Skill
{
public:
    explicit DribbleToDirectionSkill(const Common::Angle t_direction)
        : m_direction(t_direction)
    {}

    static const Id kId;

    Id id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    Common::Angle m_direction;
};
} // namespace Tyr::Soccer
