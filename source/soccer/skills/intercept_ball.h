#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
class InterceptBallSkill final : public Skill
{
public:
    InterceptBallSkill(const Common::Angle t_angle, const float t_wait_t = 0.0f) : m_angle(t_angle), m_wait_t(t_wait_t)
    {}

    static const Id kId;

    Id id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    const Common::Angle m_angle;
    const float m_wait_t;
};
} // namespace Tyr::Soccer
