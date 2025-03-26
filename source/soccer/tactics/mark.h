#pragma once

#include "tactic.h"

namespace Tyr::Soccer
{
class MarkTactic final : public Tactic
{
public:
    explicit MarkTactic(const Common::RobotState& t_opp, const float t_dist = 220.0f) : m_opp(t_opp), m_dist(t_dist)
    {}

    static const Id kId;
    Id id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    const Common::RobotState & m_opp;
    float m_dist;
};
} // namespace Tyr::Soccer
