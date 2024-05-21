#include "../ai.h"

namespace Tyr::Soccer
{
enum ball_circling_state
{
    kVeryFar = 0,
    kFar,
    kNear,
    kKick,
};

void Ai::circleBall(int t_robot_num, Common::Angle t_tagret_angle, int t_shoot_pow, int t_chip_pow, float t_precision,
                    float t_near_dis_override)
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

    static ball_circling_state state         = kVeryFar;
    static float               last_change_t = 0.0f;
    static int                 hys_bank[4]   = {0, 0, 0, 0};
    if (m_timer.time().seconds() < 0.1)
    {
        state         = kVeryFar;
        last_change_t = m_timer.time().seconds();
        hys_bank[0] = hys_bank[1] = hys_bank[2] = hys_bank[3] = 0;
        halt(t_robot_num);
        return;
    }

    if (state == kVeryFar)
    {
        Common::logDebug("STEPPPPP1");
        m_own_robot[t_robot_num].face(m_world_state.ball.position);
        VelocityProfile profile = VelocityProfile::mamooli();
        profile.max_spd         = 900.0f;
        navigate(t_robot_num, m_world_state.ball.position, profile);

        Common::debug().draw(Common::Circle{m_world_state.ball.position, very_far_ball_dis - 90.0f},
                             Common::Color::red(), false);

        if (m_own_robot[t_robot_num].state().position.distanceTo(m_world_state.ball.position) < very_far_ball_dis)
        {
            state         = kFar;
            last_change_t = m_timer.time().seconds();
        }
    }
    else if (state == kFar)
    {
        Common::logDebug("STEPPPPP2");
        m_own_robot[t_robot_num].face(m_world_state.ball.position);
        Common::Vec2 target_point = m_world_state.ball.position.circleAroundPoint(
            m_world_state.ball.position.angleWith(m_own_robot[t_robot_num].state().position), near_ball_dis);
        navigate(t_robot_num, target_point, VelocityProfile::aroom());

        Common::debug().draw(Common::Circle{m_world_state.ball.position, far_ball_dis - 90.0f}, Common::Color::pink(),
                             false);

        if (m_own_robot[t_robot_num].state().position.distanceTo(m_world_state.ball.position) < far_ball_dis)
        {
            hys_bank[0]++;
        }
        else
        {
            hys_bank[0] = 0;
        }
        if (hys_bank[0] > far_to_near_hys)
        {
            state         = kNear;
            last_change_t = m_timer.time().seconds();
        }
        else if (m_own_robot[t_robot_num].state().position.distanceTo(m_world_state.ball.position) >
                 very_far_ball_dis * shmit_coeff)
        {
            state         = kVeryFar;
            last_change_t = m_timer.time().seconds();
        }
    }
    else if (state == kNear)
    {
        Common::logDebug("STEPPPPP3");
        Common::Angle toRobot    = m_world_state.ball.position.angleWith(m_own_robot[t_robot_num].state().position);
        Common::Angle newToRobot = toRobot - t_tagret_angle;
        Common::Angle deltaAngle = Common::Angle::fromDeg(std::min(std::fabs(newToRobot.deg()), 30.0f));
        newToRobot.setDeg(std::max(0.0f, std::fabs(newToRobot.deg()) - deltaAngle.deg()) *
                          Common::sign(newToRobot.deg()));
        newToRobot = newToRobot + t_tagret_angle;

        m_own_robot[t_robot_num].face(m_world_state.ball.position);
        m_own_robot[t_robot_num].target.angle +=
            (newToRobot + Common::Angle::fromDeg(180.0f) - m_own_robot[t_robot_num].target.angle) / 2.0f;
        Common::Vec2 target_point =
            m_world_state.ball.position.circleAroundPoint(newToRobot, near_ball_dis / deltaAngle.cos());
        if (t_near_dis_override > 0)
            navigate(t_robot_num, target_point, VelocityProfile::aroom());
        else
            navigate(t_robot_num, target_point, VelocityProfile::mamooli());

        if (m_own_robot[t_robot_num].state().position.distanceTo(m_world_state.ball.position) >
            far_ball_dis * shmit_coeff)
        {
            state         = kFar;
            last_change_t = m_timer.time().seconds();
        }

        if (std::fabs(deltaAngle.deg()) < near_angle_tol)
        {
            hys_bank[0]++;
        }
        else
        {
            hys_bank[0] = 0;
        }

        if ((hys_bank[0] > near_to_kick_hys) && ((t_shoot_pow > 0) || (t_chip_pow > 0)))
        {
            state         = kKick;
            last_change_t = m_timer.time().seconds();
        }
    }

    else if (state == kKick)
    {
        Common::logDebug("STEPPPPP4");
        if (t_chip_pow > 0)
        {
            m_chip_head = m_own_robot[t_robot_num].state().angle;
        }
        m_own_robot[t_robot_num].target.angle = t_tagret_angle + Common::Angle::fromDeg(180.0f);
        navigate(t_robot_num, m_world_state.ball.position, VelocityProfile::aroom());
        if (t_shoot_pow > 0)
            m_own_robot[t_robot_num].shoot(t_shoot_pow);
        if (t_chip_pow > 0)
            m_own_robot[t_robot_num].chip(t_chip_pow);

        if (m_own_robot[t_robot_num].state().position.distanceTo(m_world_state.ball.position) >
            near_ball_dis * shmit_coeff)
        {
            state         = kFar;
            last_change_t = m_timer.time().seconds();
        }
    }
}
} // namespace Tyr::Soccer
