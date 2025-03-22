#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
class InterceptBallSkill final : public Skill
{
public:
    InterceptBallSkill(const Common::Angle t_angle) : m_angle(t_angle)
    {}

    static const Id kId;

    Id id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    const Common::Angle m_angle;
};
} // namespace Tyr::Soccer
