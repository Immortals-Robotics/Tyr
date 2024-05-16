#pragma once

#include "velocity_profile.h"

#define PREDICT_CMDS 6

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
        m_shoot    = 0;
        m_chip     = 0;
        m_dribbler = 0;
        navigated  = false;
        m_halted   = false;
    }

    void shoot(float pow);
    void chip(float pow);
    void dribble(float pow);

    void face(Common::Vec2 t_target);
    void move(Common::Vec3 motion);
    void halt();

    Common::Vec3 computeMotion(float speed, VelocityProfile::Type velocityProfile);

    Common::Vec3    currentMotion() const;
    Sender::Command currentCommand() const;

    Common::RobotState target;

    bool navigated = false;

private:
    const Common::RobotState *m_state = nullptr;

    // used in mition plan
    // TODO: merge with last_motions
    Common::Vec3 m_old_motion;

    float m_shoot    = 0;
    float m_chip     = 0;
    float m_dribbler = 0;

    // TODO: find out why we have both
    Common::Vec3 last_motions[11]  = {};
    int          m_last_motion_idx = 0;
    int          m_motion_idx      = 0;

    bool m_halted = false;
};
} // namespace Tyr::Soccer
