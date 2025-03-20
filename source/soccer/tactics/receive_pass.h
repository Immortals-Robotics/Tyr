#pragma once

#include "tactic.h"

namespace Tyr::Soccer
{
class ReceivePassTactic final : public Tactic
{
public:
    explicit ReceivePassTactic(const Common::Vec2 t_static_pos, const bool t_chip = false)
        : m_static_pos(t_static_pos), m_chip(t_chip)
    {}

    static const Id kId;
    Id              id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    Common::Vec2 m_static_pos;
    bool         m_chip;
};
} // namespace Tyr::Soccer
