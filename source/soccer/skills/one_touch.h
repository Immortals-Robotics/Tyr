#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
class OneTouchSkill final : public Skill
{
public:
    explicit OneTouchSkill(const Common::Vec2 *t_target = nullptr, const float t_kick = 0.0f, const float t_chip = 0.0f)
        : m_target(t_target), m_kick(t_kick), m_chip(t_chip)
    {}

    static const Id kId;
    Id              id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    const Common::Vec2 *m_target;

    const float m_kick;
    const float m_chip;

    Common::Angle calculateOneTouchAngle(const Robot& t_robot, Common::Vec2 t_one_touch_position);
    Common::Vec2 calculatePassPos(const Common::Vec2 &t_target, const Common::Vec2 &t_stat_pos, float t_bar = 89.0f);
};
} // namespace Tyr::Soccer
