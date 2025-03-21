#pragma once

#include "tactic.h"

namespace Tyr::Soccer
{
class GkTactic final : public Tactic
{
public:
    GkTactic() = default;

    static const Id kId;
    Id id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    bool m_intercepting = false;
    int m_hys = 0;

    // TODO: this should probably be an skill
    void shirje(Robot& t_robot);
};
} // namespace Tyr::Soccer
