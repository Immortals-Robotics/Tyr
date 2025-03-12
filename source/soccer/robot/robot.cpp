#include "robot.h"

#include "../navigation/trajectory/trajectory_2d.h"

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
