#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
class MarkToBallSkill final : public Skill
{
public:
    explicit MarkToBallSkill(const Common::RobotState &t_opp, const float t_dist = 220.0f)
        : m_opp(t_opp), m_dist(t_dist)
    {
        m_dist = std::min(1500.0f, m_dist);
    }

    static const Id kId;
    Id              id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    const Common::RobotState &m_opp;
    float                     m_dist;
};
} // namespace Tyr::Soccer
