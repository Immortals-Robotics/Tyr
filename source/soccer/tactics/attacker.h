#pragma once

#include "tactic.h"

namespace Tyr::Soccer
{
struct VelocityProfile;
enum class NavigationFlags;

class AttackerTactic final : public Tactic
{
public:
    enum class EState
    {
        None = 0,
        Interception,
        WaitForBall,
        Kick
    };

    explicit AttackerTactic(const Common::Angle t_angle, const float t_kick, const float t_chip)
        : m_angle(t_angle), m_kick(t_kick), m_chip(t_chip)
    {}

    static const Id kId;
    Id              id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    Common::Angle m_angle;
    float         m_kick;
    float         m_chip;

    static inline EState m_state = EState::None;

    NavigationFlags getNavigationFlags(const Robot &t_robot) const;
};
} // namespace Tyr::Soccer
