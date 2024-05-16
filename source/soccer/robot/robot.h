#pragma once

#include "velocity_profile.h"

#define PREDICT_CMDS 6

namespace Tyr::Soccer
{
class Robot
{
public:
    Common::RobotState target;

    int  shoot    = 0;
    int  chip     = 0;
    int  dribbler = 0;
    bool halted   = false;

    Common::Vec3 last_motions[11] = {};

    int last_motion_idx = 0;
    int motion_idx      = 0;

    Robot() = default;
    Robot(const Common::RobotState *t_state) : m_state(t_state)
    {}

    inline const Common::RobotState &state() const
    {
        return *m_state;
    }

    void Shoot(int pow);

    void Chip(int pow);

    void Dribble(int pow);

    void face(Common::Vec2 _target);

    void MoveByMotion(Common::Vec3 motion);

    Common::Vec3 ComputeMotionCommand(float speed, VelocityProfile::Type velocityProfile);

    Common::Vec3    GetCurrentMotion() const;
    Sender::Command GetCurrentCommand() const;

private:
    const Common::RobotState *m_state = nullptr;
};
} // namespace Tyr::Soccer
