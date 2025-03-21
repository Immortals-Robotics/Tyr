#pragma once

#include "tactic.h"

namespace Tyr::Soccer
{
class CircleBallTactic final : public Tactic
{
public:
    CircleBallTactic(const Common::Angle t_tagret_angle, float t_shoot_pow, const float t_chip_pow,
                     const float t_near_dis_override = -1.0f)
        : m_tagret_angle(t_tagret_angle), m_shoot_pow(t_shoot_pow), m_chip_pow(t_chip_pow),
          m_near_dis_override(t_near_dis_override)
    {}

    static const Id kId;
    Id              id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    enum class CirclingState
    {
        VeryFar = 0,
        Far,
        Near,
        Kick,
    };

    Common::Angle m_tagret_angle;
    float         m_shoot_pow;
    float         m_chip_pow;
    float         m_near_dis_override;

    // TODO: make these non-static once we persist tactics
    static inline CirclingState m_state       = CirclingState::VeryFar;
    static inline int           m_hys_bank[4] = {0, 0, 0, 0};
};
} // namespace Tyr::Soccer
