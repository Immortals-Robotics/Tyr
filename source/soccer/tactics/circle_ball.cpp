#include "circle_ball.h"

#include "../skills/halt.h"

namespace Tyr::Soccer
{
const Tactic::Id CircleBallTactic::kId = &CircleBallTactic::kId;

void CircleBallTactic::execute(Robot &t_robot)
{
    // t_tagret_angle -= 5;
    const float very_far_ball_dis = 600.0f;
    const float far_ball_dis      = 160.0f;
    const int   far_to_near_hys   = 5;
    float       near_ball_dis     = 140.0f;
    if (m_near_dis_override > 0)
        near_ball_dis = m_near_dis_override;
    const float near_angle_tol   = 4.0f;
    const int   near_to_kick_hys = 3;
    const float shmit_coeff      = 1.2f;

    if (State::timer().time().seconds() < 0.1)
    {
        m_state       = CirclingState::VeryFar;
        m_hys_bank[0] = m_hys_bank[1] = m_hys_bank[2] = m_hys_bank[3] = 0;
        HaltSkill{}.execute(t_robot);
        return;
    }

    if (m_state == CirclingState::VeryFar)
    {
        Common::logDebug("STEPPPPP1");
        t_robot.face(State::world().ball.position);
        VelocityProfile profile = VelocityProfile::mamooli();
        profile.speed           = 900.0f;
        t_robot.navigate(State::world().ball.position, profile);

        Common::debug().draw(Common::Circle{State::world().ball.position, very_far_ball_dis - 90.0f},
                             Common::Color::red(), false);

        if (t_robot.state().position.distanceTo(State::world().ball.position) < very_far_ball_dis)
        {
            m_state = CirclingState::Far;
        }
    }
    else if (m_state == CirclingState::Far)
    {
        Common::logDebug("STEPPPPP2");
        t_robot.face(State::world().ball.position);
        Common::Vec2 target_point = State::world().ball.position.circleAroundPoint(
            State::world().ball.position.angleWith(t_robot.state().position), near_ball_dis);
        t_robot.navigate(target_point, VelocityProfile::aroom());

        Common::debug().draw(Common::Circle{State::world().ball.position, far_ball_dis - 90.0f}, Common::Color::pink(),
                             false);

        if (t_robot.state().position.distanceTo(State::world().ball.position) < far_ball_dis)
        {
            m_hys_bank[0]++;
        }
        else
        {
            m_hys_bank[0] = 0;
        }
        if (m_hys_bank[0] > far_to_near_hys)
        {
            m_state = CirclingState::Near;
        }
        else if (t_robot.state().position.distanceTo(State::world().ball.position) > very_far_ball_dis * shmit_coeff)
        {
            m_state = CirclingState::VeryFar;
        }
    }
    else if (m_state == CirclingState::Near)
    {
        Common::logDebug("STEPPPPP3");
        Common::Angle toRobot    = State::world().ball.position.angleWith(t_robot.state().position);
        Common::Angle newToRobot = toRobot - m_tagret_angle;
        Common::Angle deltaAngle = Common::Angle::fromDeg(std::min(std::fabs(newToRobot.deg()), 30.0f));
        newToRobot.setDeg(std::max(0.0f, std::fabs(newToRobot.deg()) - deltaAngle.deg()) *
                          Common::sign(newToRobot.deg()));
        newToRobot = newToRobot + m_tagret_angle;

        t_robot.face(State::world().ball.position);
        t_robot.target.angle += (newToRobot + Common::Angle::fromDeg(180.0f) - t_robot.target.angle) / 2.0f;
        Common::Vec2 target_point =
            State::world().ball.position.circleAroundPoint(newToRobot, near_ball_dis / deltaAngle.cos());
        if (m_near_dis_override > 0)
            t_robot.navigate(target_point, VelocityProfile::aroom());
        else
            t_robot.navigate(target_point, VelocityProfile::mamooli());

        if (t_robot.state().position.distanceTo(State::world().ball.position) > far_ball_dis * shmit_coeff)
        {
            m_state = CirclingState::Far;
        }

        if (std::fabs(deltaAngle.deg()) < near_angle_tol)
        {
            m_hys_bank[0]++;
        }
        else
        {
            m_hys_bank[0] = 0;
        }

        if ((m_hys_bank[0] > near_to_kick_hys) && ((m_shoot_pow > 0.f) || (m_chip_pow > 0)))
        {
            m_state = CirclingState::Kick;
        }
    }

    else if (m_state == CirclingState::Kick)
    {
        Common::logDebug("STEPPPPP4");
        t_robot.target.angle = m_tagret_angle + Common::Angle::fromDeg(180.0f);
        t_robot.navigate(State::world().ball.position, VelocityProfile::aroom());
        if (m_shoot_pow > 0.f)
            t_robot.shoot(m_shoot_pow);
        if (m_chip_pow > 0)
            t_robot.chip(m_chip_pow);

        if (t_robot.state().position.distanceTo(State::world().ball.position) > near_ball_dis * shmit_coeff)
        {
            m_state = CirclingState::Far;
        }
    }
}
} // namespace Tyr::Soccer
