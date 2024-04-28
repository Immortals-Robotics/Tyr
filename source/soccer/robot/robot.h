#pragma once

#include "velocity_profile.h"

#define PREDICT_CMDS 6

namespace Tyr::Soccer
{
class Robot
{
    Common::Timer angleSendTimer;

public:
    float field_w;
    float field_h;

    float shootMult = 1.0f;

    bool               oldRobot;
    Common::RobotState State;
    Common::RobotState target;
    int                shoot, chip, Break, dribbler;
    int                Motor[4];
    unsigned char      data[32];
    struct Vector2f_V2 velocity;
    union FLOAT_32     target_orientation;
    int                serial_id, vision_id;
    bool               control_mode;
    bool               halted;
    bool               new_comm_ready;

    Common::Vec3 lastCMDs[11];
    int          CMDindex;

    int   remainingPIDParams;
    float p, i, iMax, torque;

    Robot(void);

    void sendPID(float _p, float _i, float _iMax, float _torque);

    void set_serial_id(unsigned short s_id);

    void set_vision_id(unsigned short v_id);

    void set_control_mode(bool c_mode);

    float dis(float x1, float y1, float x2, float y2);

    void Shoot(int pow);

    void Chip(int pow);

    void Dribble(int pow);

    void face(Common::Vec2 _target);

    Common::Vec3 MotionPlan(Common::RobotState state, Common::RobotState target, float speed, bool accurate,
                            Common::Vec3 *cmd, VelocityProfile *velocityProfile);

    void Move(bool accurate, float speed, VelocityProfile *velocityProfile);

    void MoveByMotion(Common::Vec3 motion);

    Common::Vec3 ComputeMotionCommand(bool accurate, float speed, VelocityProfile *velocityProfile);

    Common::Vec3 GetCurrentMotionCommand(void) const;

    void makeSendingDataReady(void);
};
} // namespace Tyr::Soccer
