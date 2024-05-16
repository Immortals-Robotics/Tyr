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

void Robot::shoot(const float pow)
{
    m_shoot = getCalibratedShootPow(state().vision_id, pow / 9.5f, shoot_coeffs);
}
void Robot::chip(const float pow)
{
    m_chip = getCalibratedShootPow(state().vision_id, pow / 2.0f, chip_coeffs);
}

void Robot::dribble(const float pow)
{
    m_dribbler = 16 * pow;
}

void Robot::face(const Common::Vec2 t_target)
{
    target.angle = state().position.angleWith(t_target);
}

Common::Vec2 Robot::computeMotion(const VelocityProfile &profile)
{
    const float field_extra_area = 200.f;

    if (std::fabs(target.position.x) > Common::field().width + field_extra_area)
        target.position.x = Common::sign(target.position.x) * (Common::field().width + field_extra_area);

    if (std::fabs(target.position.y) > Common::field().height + field_extra_area)
        target.position.y = Common::sign(target.position.y) * (Common::field().height + field_extra_area);

    Common::Vec2 motion; // The output of this function

    Common::Vec2 diff = target.position - state().position;

    Common::Vec2 tmp_max_speed = diff.abs().normalized() * profile.max_spd;

    motion.x = pow(0.6f * profile.max_dec * std::fabs(diff.x), 0.6f);
    motion.x *= Common::sign(diff.x);
    if (std::fabs(diff.x) < 5)
        motion.x = 0.0f;

    if (motion.x * m_last_motion.x <= 0)
    {
        float tmp = m_last_motion.x + profile.max_dec * Common::sign(motion.x);
        if (motion.x == 0)
            tmp = m_last_motion.x - profile.max_dec * Common::sign(m_last_motion.x);

        if (tmp * motion.x > 0)
        {
            tmp = std::min(profile.max_acc, std::fabs(tmp)) * Common::sign(motion.x);
            if (std::fabs(tmp) > std::fabs(motion.x))
                tmp = motion.x;
        }

        motion.x = tmp;
    }
    else
    {
        if (std::fabs(motion.x) > std::fabs(m_last_motion.x) + profile.max_acc)
        {
            motion.x = (std::fabs(m_last_motion.x) + profile.max_acc) * Common::sign(motion.x);
        }
        else if (std::fabs(motion.x) < std::fabs(m_last_motion.x) - profile.max_dec)
        {
            motion.x = (std::fabs(m_last_motion.x) - profile.max_dec) * Common::sign(motion.x);
        }
        if (std::fabs(motion.x) > tmp_max_speed.x)
        {
            if (Common::sign(motion.x) == 0)
            { // NOT ENTERING HERE!!!!
                motion.x = std::max(std::fabs(m_last_motion.x) - profile.max_dec, std::fabs(tmp_max_speed.x)) *
                           Common::sign(m_last_motion.x);
            }
            else
                motion.x = std::max(std::fabs(m_last_motion.x) - profile.max_dec, std::fabs(tmp_max_speed.x)) *
                           Common::sign(motion.x);
        }
    }

    motion.y = pow(0.6f * profile.max_dec * std::fabs(diff.y), 0.6f);
    motion.y *= Common::sign(diff.y);
    if (std::fabs(diff.y) < 5)
    {
        motion.y = 0; // std::max(0,std::fabs(m_last_motion.y)-profile.max_dec.y)*Common::sign(motion.y);
    }
    if (motion.y * m_last_motion.y <= 0)
    {
        float tmp = m_last_motion.y + profile.max_dec * Common::sign(motion.y);
        if (motion.y == 0)
            tmp = m_last_motion.y - profile.max_dec * Common::sign(m_last_motion.y);
        if (tmp * motion.y > 0)
        {
            tmp = std::min(profile.max_acc, std::fabs(tmp)) * Common::sign(motion.y);
            if (std::fabs(tmp) > std::fabs(motion.y))
                tmp = motion.y;
        }

        motion.y = tmp;
    }
    else
    {
        if (std::fabs(motion.y) > std::fabs(m_last_motion.y) + profile.max_acc)
        {
            motion.y = (std::fabs(m_last_motion.y) + profile.max_acc) * Common::sign(motion.y);
        }
        else if (std::fabs(motion.y) < std::fabs(m_last_motion.y) - profile.max_dec)
        {
            motion.y = (std::fabs(m_last_motion.y) - profile.max_dec) * Common::sign(motion.y);
        }
        if (std::fabs(motion.y) > tmp_max_speed.y)
        {
            if (Common::sign(motion.y) == 0)
                motion.y = std::max(std::fabs(m_last_motion.y) - profile.max_dec, std::fabs(tmp_max_speed.y)) *
                           Common::sign(m_last_motion.y);
            else
                motion.y = std::max(std::fabs(m_last_motion.y) - profile.max_dec, std::fabs(tmp_max_speed.y)) *
                           Common::sign(motion.y);
        }

        if (std::fabs(motion.y) > std::fabs(m_last_motion.y))
            if (std::fabs(motion.y - m_last_motion.y) > profile.max_acc * 1.1)
                Common::logDebug("    gaz nade    {}        <    {}", profile.max_acc,
                                 std::fabs(motion.y - m_last_motion.y));

        if (std::fabs(motion.y) < std::fabs(m_last_motion.y))
            if (std::fabs(motion.y - m_last_motion.y) > profile.max_dec * 1.1)
                Common::logDebug("    tormoz nakon    {}        <    ", profile.max_dec,
                                 std::fabs(motion.y - m_last_motion.y));
    }

    return motion;
}

void Robot::move(Common::Vec2 motion)
{
    if (m_navigated && state().seen_state != Common::SeenState::CompletelyOut)
    {
        Common::logWarning("Robot {} is navigated more than once", state().vision_id);
    }

    motion.x = std::clamp(motion.x, -100.0f, 100.0f);
    motion.y = std::clamp(motion.y, -100.0f, 100.0f);

    m_last_motion = motion;

    m_navigated = true;
}

void Robot::halt()
{
    target.angle    = state().angle;
    target.position = state().position;

    m_last_motion = Common::Vec2();

    m_shoot    = 0.0f;
    m_chip     = 0.0f;
    m_dribbler = 0.0f;

    m_halted = true;
}

Common::Vec2 Robot::currentMotion() const
{
    return m_last_motion;
}

Sender::Command Robot::currentCommand() const
{
    Sender::Command command;
    command.vision_id     = state().vision_id;
    command.halted        = m_halted;
    command.motion        = currentMotion();
    command.current_angle = state().angle;
    command.target_angle  = target.angle;
    command.shoot         = m_shoot;
    command.chip          = m_chip;
    command.dribbler      = m_dribbler;

    return command;
}
} // namespace Tyr::Soccer
