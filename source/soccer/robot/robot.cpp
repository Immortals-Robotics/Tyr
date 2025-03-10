#include "robot.h"

namespace Tyr::Soccer
{
void Robot::shoot(const float pow)
{
    m_shoot = pow * Common::config().soccer.kick_tune_coef;
}
void Robot::chip(const float pow)
{
    m_chip = pow * Common::config().soccer.chip_tune_coef;
}

void Robot::dribble(const float pow)
{
    m_dribbler = 16 * pow;
}

void Robot::face(const Common::Vec2 t_target)
{
    target.angle = state().position.angleWith(t_target);
}

static float computeMotion1D(const float v0, const float delta, const float acc, const float dec, const float max_speed)
{
    float result = 0.f;

    result = pow(7.6f * dec * std::fabs(delta), 0.6f);
    result *= Common::sign(delta);
    if (std::fabs(delta) < 5)
        result = 0.0f;

    if (result * v0 <= 0)
    {
        float tmp = result == 0.0f
            ? v0 - dec * Common::sign(v0)
            : v0 + dec * Common::sign(result);

        if (tmp * result > 0)
        {
            tmp = std::min(acc, std::fabs(tmp)) * Common::sign(tmp);
            if (std::fabs(tmp) > std::fabs(result))
                tmp = result;
        }

        result = tmp;
    }
    else
    {
        if (std::fabs(result) > std::fabs(v0) + acc)
        {
            result = (std::fabs(v0) + acc) * Common::sign(result);
        }
        else if (std::fabs(result) < std::fabs(v0) - dec)
        {
            result = (std::fabs(v0) - dec) * Common::sign(result);
        }
        if (std::fabs(result) > max_speed)
        {
            result = std::max(std::fabs(v0) - dec, std::fabs(max_speed)) * Common::sign(result);
        }
    }

    return result;
}

Common::Vec2 Robot::computeMotion(const VelocityProfile &profile) const
{
    const Common::Vec2 delta = target.position - state().position;
    const Common::Vec2 tmp_max_speed = delta.abs().normalized() * profile.speed;

    const float dt = 1.f / Common::config().vision.vision_frame_rate;
    const float acc = profile.acceleration * dt;
    const float dec = profile.deceleration * dt;

    return {
        computeMotion1D(m_last_motion.x, delta.x, acc, dec, tmp_max_speed.x),
        computeMotion1D(m_last_motion.y, delta.y, acc, dec, tmp_max_speed.y)
    };
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
