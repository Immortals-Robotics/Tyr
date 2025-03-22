#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
class OneTouchSkill final : public Skill
{
public:
    explicit OneTouchSkill(const bool t_chip = false, const Common::Vec2 *t_target = nullptr)
        : m_chip(t_chip), m_target(t_target)
    {}

    static const Id kId;
    Id              id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    bool                m_chip;
    const Common::Vec2 *m_target;

    Common::Angle calculateOneTouchAngle(const Robot& t_robot, Common::Vec2 t_one_touch_position);
    Common::Vec2 calculatePassPos(const Common::Vec2 &t_target, const Common::Vec2 &t_stat_pos, float t_bar = 89.0f);
};
} // namespace Tyr::Soccer
