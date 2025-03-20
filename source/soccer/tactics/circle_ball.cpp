#include "../ai.h"

#include "../skills/halt.h"

namespace Tyr::Soccer
{
enum ball_circling_state
{
    kVeryFar = 0,
    kFar,
    kNear,
    kKick,
};

void Ai::circleBall(Robot &t_robot, const Common::Angle t_tagret_angle, float t_shoot_pow, const int t_chip_pow,
                    const float t_near_dis_override)
{
    // t_tagret_angle -= 5;
    const float very_far_ball_dis = 600.0f;
    const float far_ball_dis      = 160.0f;
    const int   far_to_near_hys   = 5;
    float       near_ball_dis     = 140.0f;
    if (t_near_dis_override > 0)
        near_ball_dis = t_near_dis_override;
    const float near_angle_tol   = 4.0f;
    const int   near_to_kick_hys = 3;
    const float shmit_coeff      = 1.2f;

    static ball_circling_state state       = kVeryFar;
    static int                 hys_bank[4] = {0, 0, 0, 0};
    if (m_timer.time().seconds() < 0.1)
    {
        state       = kVeryFar;
        hys_bank[0] = hys_bank[1] = hys_bank[2] = hys_bank[3] = 0;
        HaltSkill{}.execute(t_robot);
        return;
    }

    if (state == kVeryFar)
    {
        Common::logDebug("STEPPPPP1");
        t_robot.face(m_world_state.ball.position);
        VelocityProfile profile = VelocityProfile::mamooli();
        profile.speed           = 900.0f;
        t_robot.navigate(m_world_state.ball.position, profile);

        Common::debug().draw(Common::Circle{m_world_state.ball.position, very_far_ball_dis - 90.0f},
                             Common::Color::red(), false);

        if (t_robot.state().position.distanceTo(m_world_state.ball.position) < very_far_ball_dis)
        {
            state = kFar;
        }
    }
    else if (state == kFar)
    {
        Common::logDebug("STEPPPPP2");
        t_robot.face(m_world_state.ball.position);
        Common::Vec2 target_point = m_world_state.ball.position.circleAroundPoint(
            m_world_state.ball.position.angleWith(t_robot.state().position), near_ball_dis);
        t_robot.navigate(target_point, VelocityProfile::aroom());

        Common::debug().draw(Common::Circle{m_world_state.ball.position, far_ball_dis - 90.0f}, Common::Color::pink(),
                             false);

        if (t_robot.state().position.distanceTo(m_world_state.ball.position) < far_ball_dis)
        {
            hys_bank[0]++;
        }
        else
        {
            hys_bank[0] = 0;
        }
        if (hys_bank[0] > far_to_near_hys)
        {
            state = kNear;
        }
        else if (t_robot.state().position.distanceTo(m_world_state.ball.position) > very_far_ball_dis * shmit_coeff)
        {
            state = kVeryFar;
        }
    }
    else if (state == kNear)
    {
        Common::logDebug("STEPPPPP3");
        Common::Angle toRobot    = m_world_state.ball.position.angleWith(t_robot.state().position);
        Common::Angle newToRobot = toRobot - t_tagret_angle;
        Common::Angle deltaAngle = Common::Angle::fromDeg(std::min(std::fabs(newToRobot.deg()), 30.0f));
        newToRobot.setDeg(std::max(0.0f, std::fabs(newToRobot.deg()) - deltaAngle.deg()) *
                          Common::sign(newToRobot.deg()));
        newToRobot = newToRobot + t_tagret_angle;

        t_robot.face(m_world_state.ball.position);
        t_robot.target.angle += (newToRobot + Common::Angle::fromDeg(180.0f) - t_robot.target.angle) / 2.0f;
        Common::Vec2 target_point =
            m_world_state.ball.position.circleAroundPoint(newToRobot, near_ball_dis / deltaAngle.cos());
        if (t_near_dis_override > 0)
            t_robot.navigate(target_point, VelocityProfile::aroom());
        else
            t_robot.navigate(target_point, VelocityProfile::mamooli());

        if (t_robot.state().position.distanceTo(m_world_state.ball.position) > far_ball_dis * shmit_coeff)
        {
            state = kFar;
        }

        if (std::fabs(deltaAngle.deg()) < near_angle_tol)
        {
            hys_bank[0]++;
        }
        else
        {
            hys_bank[0] = 0;
        }

        if ((hys_bank[0] > near_to_kick_hys) && ((t_shoot_pow > 0.f) || (t_chip_pow > 0)))
        {
            state = kKick;
        }
    }

    else if (state == kKick)
    {
        Common::logDebug("STEPPPPP4");
        t_robot.target.angle = t_tagret_angle + Common::Angle::fromDeg(180.0f);
        t_robot.navigate(m_world_state.ball.position, VelocityProfile::aroom());
        if (t_shoot_pow > 0.f)
            t_robot.shoot(t_shoot_pow);
        if (t_chip_pow > 0)
            t_robot.chip(t_chip_pow);

        if (t_robot.state().position.distanceTo(m_world_state.ball.position) > near_ball_dis * shmit_coeff)
        {
            state = kFar;
        }
    }
}
} // namespace Tyr::Soccer
