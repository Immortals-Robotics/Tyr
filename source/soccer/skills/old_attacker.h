#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
enum class NavigationFlags;

class OldAttackerSkill final : public Skill
{
public:
    explicit OldAttackerSkill(const Common::Angle t_angle, const float   t_kick, const float t_chip, const bool t_kiss,
                              const bool          t_dribbler, const bool t_precise = false, const bool t_is_gk = false)
        : m_angle(t_angle), m_kick(t_kick), m_chip(t_chip), m_kiss(t_kiss), m_dribbler(t_dribbler),
          m_precise(t_precise), m_is_gk(t_is_gk)
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
    bool          m_kiss;
    bool          m_dribbler;
    bool          m_precise;
    bool          m_is_gk;

    static inline bool m_circle_reached_behind_ball = false;
    static inline int  m_elendil                    = 0;

    NavigationFlags getNavigationFlags(const Robot &t_robot) const;
};
} // namespace Tyr::Soccer
