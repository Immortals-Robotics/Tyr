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

void Robot::move(const Trajectory2D& trajectory)
{
    drawTrajectory(trajectory, Common::Color::black());

    if (m_navigated && state().seen_state != Common::SeenState::CompletelyOut)
    {
        Common::logWarning("Robot {} is navigated more than once", state().vision_id);
    }

    m_trajectory = trajectory;

    m_navigated = true;
}

void Robot::halt()
{
    target.angle    = state().angle;
    target.position = state().position;

    m_trajectory = {};

    m_shoot    = 0.0f;
    m_chip     = 0.0f;
    m_dribbler = 0.0f;

    m_halted = true;
}

void Robot::fullBeak(const float acc_factor)
{
    VelocityProfile profile = VelocityProfile::mamooli();
    profile.acceleration *= acc_factor;

    const Trajectory2D trajectory = Trajectory2D::makeFullStopTrajectory(state().position, currentMotion(), profile);
    move(trajectory);
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
