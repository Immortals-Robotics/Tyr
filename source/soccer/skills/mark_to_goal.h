#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
class MarkToGoalSkill final : public Skill
{
public:
    explicit MarkToGoalSkill(const Common::RobotState &t_opp, float t_dist = 220.0f, bool t_def_area_mark = false)
        : m_opp(t_opp), m_dist(t_dist), m_def_area_mark(t_def_area_mark)
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
    bool                      m_def_area_mark;
};
} // namespace Tyr::Soccer
