#include "robot.h"

namespace Tyr::Soccer
{
static float shoot_coeffs[Common::Setting::kMaxRobots][2] = {
    {6.68088f, 4.88984f},   //  0
    {5.47922f, 7.44389f},   //  1
    {8.42397f, 11.95175f},  //  2
    {7.81809f, 3.22943f},   //  3
    {13.42397f, 17.95175f}, //  4
    {4.05175f, 9.08842f},   //  5
    {6.81159f, 12.43478},   //  6
    {4.05175f, 9.08842f},   //  7
    {0, 0},                 //  8*
    {0, 0},                 //  9*
    {0, 0},                 // 10*
    {0, 0},                 // 11*
};

static float chip_coeffs[Common::Setting::kMaxRobots][2] = {
    {5.41180f, 12.58416f}, //  0
    {4.19252f, 15.25629f}, //  1
    {9.14946f, 8.34962f},  //  2
    {4.90275f, 16.94857f}, //  3
    {5.48581f, 9.45330f},  //  4
    {5.48581f, 9.45330f},  //  5
    {7.88439f, 6.42894f},  //  6
    {5.48581f, 9.45330f},  //  7
    {0, 0},                //  8*
    {0, 0},                //  9*
    {0, 0},                // 10*
    {0, 0},                // 11*
};

float getCalibratedShootPow(int vision_id, float raw_shoot, float coeffs[Common::Setting::kMaxRobots][2])
{
    if (raw_shoot <= 0)
    {
        return 0;
    }
    vision_id = std::min((int) Common::Setting::kMaxRobots, std::max(0, vision_id));

    const float a = coeffs[vision_id][0];
    const float b = coeffs[vision_id][1];

    raw_shoot = std::min(150.0f, std::max(0.0f, raw_shoot));

    float calib_shoot = a * raw_shoot + b;
    calib_shoot       = std::min(100.0f, std::max(0.0f, calib_shoot));

    return calib_shoot;
}

Robot::Robot()
{
    motion_idx = 0;
    for (int i = 0; i < 10; i++)
        last_motions[i] = Common::Vec3();

    shoot    = 0;
    chip     = 0;
    dribbler = 0;
    halted   = false;
}

void Robot::setVisionId(unsigned short v_id)
{
    vision_id = v_id;
}

void Robot::Shoot(int pow)
{
    shoot = getCalibratedShootPow(vision_id, pow / 9.5f, shoot_coeffs);
}
void Robot::Chip(int pow)
{
    chip = getCalibratedShootPow(vision_id, pow / 2.0f, chip_coeffs);
}

void Robot::Dribble(int pow)
{
    dribbler = 16 * pow;
}

void Robot::face(Common::Vec2 _target)
{
    target.angle = state().position.angleWith(_target);
}

Common::Vec3 Robot::ComputeMotionCommand(float speed, const VelocityProfile::Type velocityProfileType)
{
    const VelocityProfile profile(velocityProfileType);

    const float field_extra_area = 200.f;

    if (std::fabs(target.position.x) > Common::field().width + field_extra_area)
        target.position.x = Common::sign(target.position.x) * (Common::field().width + field_extra_area);

    if (std::fabs(target.position.y) > Common::field().height + field_extra_area)
        target.position.y = Common::sign(target.position.y) * (Common::field().height + field_extra_area);

    speed = std::clamp(speed, 0.0f, 100.0f);

    static Common::Vec3 oldAns[12] = {};

    Common::Vec3 motion; // The output of this function
    motion.z = target.angle.deg();
    motion.z /= 5.0f;

    Common::Vec2 diff = target.position - state().position;

    Common::Vec2 tmp_max_speed;

    float target_dis = Common::Vec2(0.0f).distanceTo(diff);
    tmp_max_speed.x  = profile.max_spd.x * std::fabs(diff.x) / target_dis;

    tmp_max_speed.y = profile.max_spd.y * std::fabs(diff.y) / target_dis;

    if (tmp_max_speed.length() > speed)
    {
        tmp_max_speed = tmp_max_speed.normalized() * speed;
    }

    float KP   = 0.10;
    float Pdis = 0;

    motion.x = speed;
    if (std::fabs(diff.x) < Pdis) // always FALSE (?)
    {
        motion.x = KP * profile.max_dec.x * std::fabs(diff.x);
    }
    else
    {
        motion.x = pow(0.6f * profile.max_dec.x * std::fabs(diff.x), 0.6f);
    }
    motion.x *= Common::sign(diff.x);
    if (std::fabs(diff.x) < 5)
        motion.x = 0.0f;

    if (motion.x * oldAns[state().vision_id].x <= 0)
    {
        float tmp = oldAns[state().vision_id].x + profile.max_dec.x * Common::sign(motion.x);
        if (motion.x == 0)
            tmp = oldAns[state().vision_id].x - profile.max_dec.x * Common::sign(oldAns[state().vision_id].x);

        if (tmp * motion.x > 0)
        {
            tmp = std::min(profile.max_acc.x, std::fabs(tmp)) * Common::sign(motion.x);
            if (std::fabs(tmp) > std::fabs(motion.x))
                tmp = motion.x;
        }

        motion.x = tmp;
    }
    else
    {
        if (std::fabs(motion.x) > std::fabs(oldAns[state().vision_id].x) + profile.max_acc.x)
        {
            motion.x = (std::fabs(oldAns[state().vision_id].x) + profile.max_acc.x) * Common::sign(motion.x);
        }
        else if (std::fabs(motion.x) < std::fabs(oldAns[state().vision_id].x) - profile.max_dec.x)
        {
            motion.x = (std::fabs(oldAns[state().vision_id].x) - profile.max_dec.x) * Common::sign(motion.x);
        }
        if (std::fabs(motion.x) > tmp_max_speed.x)
        {
            if (Common::sign(motion.x) == 0)
            { // NOT ENTERING HERE!!!!
                motion.x =
                    std::max(std::fabs(oldAns[state().vision_id].x) - profile.max_dec.x, std::fabs(tmp_max_speed.x)) *
                    Common::sign(oldAns[state().vision_id].x);
            }
            else
                motion.x =
                    std::max(std::fabs(oldAns[state().vision_id].x) - profile.max_dec.x, std::fabs(tmp_max_speed.x)) *
                    Common::sign(motion.x);
        }
    }

    motion.y = speed;
    if (std::fabs(diff.y) < Pdis)
    {
        motion.y = KP * profile.max_dec.y * std::fabs(diff.y);
    }
    else
    {
        motion.y = pow(0.6f * profile.max_dec.y * std::fabs(diff.y), 0.6f);
    }
    motion.y *= Common::sign(diff.y);
    if (std::fabs(diff.y) < 5)
    {
        motion.y = 0; // std::max(0,std::fabs(oldAns[state().vision_id].y)-profile.max_dec.y)*Common::sign(motion.y);
    }
    if (motion.y * oldAns[state().vision_id].y <= 0)
    {
        float tmp = oldAns[state().vision_id].y + profile.max_dec.y * Common::sign(motion.y);
        if (motion.y == 0)
            tmp = oldAns[state().vision_id].y - profile.max_dec.y * Common::sign(oldAns[state().vision_id].y);
        if (tmp * motion.y > 0)
        {
            tmp = std::min(profile.max_acc.y, std::fabs(tmp)) * Common::sign(motion.y);
            if (std::fabs(tmp) > std::fabs(motion.y))
                tmp = motion.y;
        }

        motion.y = tmp;
    }
    else
    {
        if (std::fabs(motion.y) > std::fabs(oldAns[state().vision_id].y) + profile.max_acc.y)
        {
            motion.y = (std::fabs(oldAns[state().vision_id].y) + profile.max_acc.y) * Common::sign(motion.y);
        }
        else if (std::fabs(motion.y) < std::fabs(oldAns[state().vision_id].y) - profile.max_dec.y)
        {
            motion.y = (std::fabs(oldAns[state().vision_id].y) - profile.max_dec.y) * Common::sign(motion.y);
        }
        if (std::fabs(motion.y) > tmp_max_speed.y)
        {
            if (Common::sign(motion.y) == 0)
                motion.y =
                    std::max(std::fabs(oldAns[state().vision_id].y) - profile.max_dec.y, std::fabs(tmp_max_speed.y)) *
                    Common::sign(oldAns[state().vision_id].y);
            else
                motion.y =
                    std::max(std::fabs(oldAns[state().vision_id].y) - profile.max_dec.y, std::fabs(tmp_max_speed.y)) *
                    Common::sign(motion.y);
        }

        if (std::fabs(motion.y) > std::fabs(oldAns[state().vision_id].y))
            if (std::fabs(motion.y - oldAns[state().vision_id].y) > profile.max_acc.y * 1.1)
                Common::logDebug("    gaz nade    {}        <    {}", profile.max_acc.y,
                                 std::fabs(motion.y - oldAns[state().vision_id].y));

        if (std::fabs(motion.y) < std::fabs(oldAns[state().vision_id].y))
            if (std::fabs(motion.y - oldAns[state().vision_id].y) > profile.max_dec.y * 1.1)
                Common::logDebug("    tormoz nakon    {}        <    ", profile.max_dec.y,
                                 std::fabs(motion.y - oldAns[state().vision_id].y));
    }

    oldAns[state().vision_id] = motion;

    target.velocity.x = 0;
    target.velocity.y = 0;

    return motion;
}

void Robot::MoveByMotion(Common::Vec3 motion)
{
    motion.x = std::clamp(motion.x, -100.0f, 100.0f);
    motion.y = std::clamp(motion.y, -100.0f, 100.0f);

    last_motions[motion_idx] = motion;

    last_motion_idx = motion_idx;
    motion_idx++;
    if (motion_idx > PREDICT_CMDS - 1)
        motion_idx = 0;
}

Common::Vec3 Robot::GetCurrentMotion() const
{
    return last_motions[last_motion_idx];
}

Sender::Command Robot::GetCurrentCommand() const
{
    return {.vision_id     = vision_id,
            .halted        = halted,
            .motion        = GetCurrentMotion(),
            .current_angle = state().angle,
            .target_angle  = target.angle,
            .shoot         = shoot,
            .chip          = chip,
            .dribbler      = dribbler};
}
} // namespace Tyr::Soccer
