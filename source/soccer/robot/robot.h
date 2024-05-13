#pragma once

#include "velocity_profile.h"

#define PREDICT_CMDS 6

namespace Tyr::Soccer
{
class Robot
{
public:
    Common::RobotState target;
    int                shoot, chip, dribbler;
    int                vision_id;
    bool               halted;

    Common::Vec3 last_motions[11];

    int last_motion_idx = 0;
    int motion_idx;

    Robot() = default;
    Robot(const Common::WorldState *t_world_state);

    inline const Common::RobotState &state() const
    {
        return m_world_state->own_robot[vision_id];
    }

    void setVisionId(unsigned short v_id);

    void Shoot(int pow);

    void Chip(int pow);

    void Dribble(int pow);

    void face(Common::Vec2 _target);

    void MoveByMotion(Common::Vec3 motion);

    Common::Vec3 ComputeMotionCommand(float speed, VelocityProfile::Type velocityProfile);

    Common::Vec3    GetCurrentMotion() const;
    Sender::Command GetCurrentCommand() const;

private:
    const Common::WorldState *m_world_state = nullptr;
};
} // namespace Tyr::Soccer
