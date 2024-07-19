#include "robot.h"

namespace Tyr::Soccer
{
void Robot::shoot(const float pow)
{
    m_shoot = pow;
}
void Robot::chip(const float pow)
{
    m_chip = pow;
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
    if (std::fabs(target.position.x) > Common::field().width + Common::field().boundary_width)
        target.position.x = Common::sign(target.position.x) * (Common::field().width + Common::field().boundary_width);

    if (std::fabs(target.position.y) > Common::field().height + Common::field().boundary_width)
        target.position.y = Common::sign(target.position.y) * (Common::field().height + Common::field().boundary_width);

    Common::Vec2 motion; // The output of this function

    Common::Vec2 diff = target.position - state().position;

    Common::Vec2 tmp_max_speed = diff.abs().normalized() * profile.max_spd;

    const float acc = profile.max_acc / Common::config().vision.vision_frame_rate;
    const float dec = profile.max_dec / Common::config().vision.vision_frame_rate;

    motion.x = pow(7.6f * dec * std::fabs(diff.x), 0.6f);
    motion.x *= Common::sign(diff.x);
    if (std::fabs(diff.x) < 5)
        motion.x = 0.0f;

    if (motion.x * m_last_motion.x <= 0)
    {
        float tmp = m_last_motion.x + dec * Common::sign(motion.x);
        if (motion.x == 0)
            tmp = m_last_motion.x - dec * Common::sign(m_last_motion.x);

        if (tmp * motion.x > 0)
        {
            tmp = std::min(acc, std::fabs(tmp)) * Common::sign(motion.x);
            if (std::fabs(tmp) > std::fabs(motion.x))
                tmp = motion.x;
        }

        motion.x = tmp;
    }
    else
    {
        if (std::fabs(motion.x) > std::fabs(m_last_motion.x) + acc)
        {
            motion.x = (std::fabs(m_last_motion.x) + acc) * Common::sign(motion.x);
        }
        else if (std::fabs(motion.x) < std::fabs(m_last_motion.x) - dec)
        {
            motion.x = (std::fabs(m_last_motion.x) - dec) * Common::sign(motion.x);
        }
        if (std::fabs(motion.x) > tmp_max_speed.x)
        {
            motion.x = std::max(std::fabs(m_last_motion.x) - dec, std::fabs(tmp_max_speed.x)) * Common::sign(motion.x);
        }
    }

    motion.y = pow(7.6f * dec * std::fabs(diff.y), 0.6f);
    motion.y *= Common::sign(diff.y);
    if (std::fabs(diff.y) < 5)
    {
        motion.y = 0; // std::max(0,std::fabs(m_last_motion.y)-dec.y)*Common::sign(motion.y);
    }
    if (motion.y * m_last_motion.y <= 0)
    {
        float tmp = m_last_motion.y + dec * Common::sign(motion.y);
        if (motion.y == 0)
            tmp = m_last_motion.y - dec * Common::sign(m_last_motion.y);
        if (tmp * motion.y > 0)
        {
            tmp = std::min(acc, std::fabs(tmp)) * Common::sign(motion.y);
            if (std::fabs(tmp) > std::fabs(motion.y))
                tmp = motion.y;
        }

        motion.y = tmp;
    }
    else
    {
        if (std::fabs(motion.y) > std::fabs(m_last_motion.y) + acc)
        {
            motion.y = (std::fabs(m_last_motion.y) + acc) * Common::sign(motion.y);
        }
        else if (std::fabs(motion.y) < std::fabs(m_last_motion.y) - dec)
        {
            motion.y = (std::fabs(m_last_motion.y) - dec) * Common::sign(motion.y);
        }
        if (std::fabs(motion.y) > tmp_max_speed.y)
        {
            if (Common::sign(motion.y) == 0)
                motion.y = std::max(std::fabs(m_last_motion.y) - dec, std::fabs(tmp_max_speed.y)) *
                           Common::sign(m_last_motion.y);
            else
                motion.y =
                    std::max(std::fabs(m_last_motion.y) - dec, std::fabs(tmp_max_speed.y)) * Common::sign(motion.y);
        }

        if (std::fabs(motion.y) > std::fabs(m_last_motion.y))
            if (std::fabs(motion.y - m_last_motion.y) > acc * 1.1)
                Common::logDebug("    gaz nade    {}        <    {}", acc, std::fabs(motion.y - m_last_motion.y));

        if (std::fabs(motion.y) < std::fabs(m_last_motion.y))
            if (std::fabs(motion.y - m_last_motion.y) > dec * 1.1)
                Common::logDebug("    tormoz nakon    {}        <    ", dec, std::fabs(motion.y - m_last_motion.y));
    }

    return motion;
}

void Robot::move(Common::Vec2 motion)
{
    if (m_navigated && state().seen_state != Common::SeenState::CompletelyOut)
    {
        Common::logWarning("Robot {} is navigated more than once", state().vision_id);
    }

    motion.x = std::clamp(motion.x, -4500.0f, 4500.0f);
    motion.y = std::clamp(motion.y, -4500.0f, 4500.0f);

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
