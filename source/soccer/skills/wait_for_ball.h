#pragma once

#include "skill.h"

namespace Tyr::Soccer
{
class WaitForBallSkill final : public Skill
{
public:
    enum class WaitMode
    {
        ClosestToRobot, // wait at the closest point on the ball path to the robot (current position)
        Quickest,       // wait at the earliest reachable point on the ball path
    };

    WaitForBallSkill(const Common::Vec2 t_static_pos, const WaitMode t_mode = WaitMode::ClosestToRobot)
        : m_static_pos(t_static_pos), m_mode(t_mode)
    {}

    static const Id kId;

    Id id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    Common::Vec2 m_static_pos;
    WaitMode     m_mode;
};
} // namespace Tyr::Soccer
