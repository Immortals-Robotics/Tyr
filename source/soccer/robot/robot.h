#pragma once

#include "velocity_profile.h"

namespace Tyr::Soccer
{
class Robot
{
public:
    Robot() = default;
    Robot(const Common::RobotState *t_state) : m_state(t_state)
    {}

    inline const Common::RobotState &state() const
    {
        return *m_state;
    }

    inline void reset()
    {
        m_shoot     = 0;
        m_chip      = 0;
        m_dribbler  = 0;
        m_navigated = false;
        m_halted    = false;
    }

    void shoot(float pow);
    void chip(float pow);
    void dribble(float pow);

    void face(Common::Vec2 t_target);
    void move(Common::Vec2 motion);
    void halt();

    [[nodiscard]] bool navigated() const
    {
        return m_navigated;
    }

    [[nodiscard]] Common::Vec2 computeMotion(float speed, VelocityProfile::Type velocityProfile);

    [[nodiscard]] Common::Vec2    currentMotion() const;
    [[nodiscard]] Sender::Command currentCommand() const;

    Common::RobotState target;

private:
    const Common::RobotState *m_state = nullptr;

    // used in mition plan
    // TODO: merge with last_motions
    Common::Vec2 m_old_motion;

    Common::Vec2 m_last_motion;

    float m_shoot    = 0;
    float m_chip     = 0;
    float m_dribbler = 0;

    bool m_navigated = false;
    bool m_halted    = false;
};
} // namespace Tyr::Soccer
