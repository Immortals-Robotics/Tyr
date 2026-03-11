#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
enum class NavigationFlags;

class KickBallSkill final : public Skill
{
public:
    explicit KickBallSkill(const Common::Angle t_angle, const float t_kick, const float t_chip, const bool t_is_gk = false)
        : m_angle(t_angle), m_kick(t_kick), m_chip(t_chip), m_is_gk(t_is_gk)
    {}

    static const Id kId;

    Id id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    Common::Angle m_angle;
    float         m_kick;
    float         m_chip;
    bool          m_is_gk;

    NavigationFlags getNavigationFlags(const Robot &t_robot) const;
};
} // namespace Tyr::Soccer
