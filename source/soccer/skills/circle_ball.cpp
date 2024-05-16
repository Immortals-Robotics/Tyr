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

void Ai::circle_ball(int robot_num, Common::Angle tagret_angle, int shoot_pow, int chip_pow, float precision,
                     float near_dis_override)
{
    // tagret_angle -= 5;
    const float very_far_ball_dis = 600.0f;
    const float far_ball_dis      = 160.0f;
    const int   far_to_near_hys   = 5;
    float       near_ball_dis     = 140.0f;
    if (near_dis_override > 0)
        near_ball_dis = near_dis_override;
    const float near_angle_tol   = 4.0f;
    const int   near_to_kick_hys = 3;
    const float shmit_coeff      = 1.2f;

    static ball_circling_state state         = kVeryFar;
    static float               last_change_t = 0.0f;
    static int                 hys_bank[4]   = {0, 0, 0, 0};
    if (timer.time() < 0.1)
    {
        state         = kVeryFar;
        last_change_t = timer.time();
        hys_bank[0] = hys_bank[1] = hys_bank[2] = hys_bank[3] = 0;
        Halt(robot_num);
        return;
    }

    if (state == kVeryFar)
    {
        Common::logDebug("STEPPPPP1");
        OwnRobot[robot_num].face(m_world_state.ball.position);
        VelocityProfile profile = VelocityProfile::mamooli();
        profile.max_spd         = 900.0f;
        navigate(robot_num, m_world_state.ball.position, profile);

        Common::debug().draw(Common::Circle{m_world_state.ball.position, very_far_ball_dis - 90.0f},
                             Common::Color::red(), false);

        if (OwnRobot[robot_num].state().position.distanceTo(m_world_state.ball.position) < very_far_ball_dis)
        {
            state         = kFar;
            last_change_t = timer.time();
        }
    }
    else if (state == kFar)
    {
        Common::logDebug("STEPPPPP2");
        OwnRobot[robot_num].face(m_world_state.ball.position);
        Common::Vec2 target_point = m_world_state.ball.position.circleAroundPoint(
            m_world_state.ball.position.angleWith(OwnRobot[robot_num].state().position), near_ball_dis);
        navigate(robot_num, target_point, VelocityProfile::aroom());

        Common::debug().draw(Common::Circle{m_world_state.ball.position, far_ball_dis - 90.0f}, Common::Color::pink(),
                             false);

        if (OwnRobot[robot_num].state().position.distanceTo(m_world_state.ball.position) < far_ball_dis)
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
            last_change_t = timer.time();
        }
        else if (OwnRobot[robot_num].state().position.distanceTo(m_world_state.ball.position) >
                 very_far_ball_dis * shmit_coeff)
        {
            state         = kVeryFar;
            last_change_t = timer.time();
        }
    }
    else if (state == kNear)
    {
        Common::logDebug("STEPPPPP3");
        Common::Angle toRobot    = m_world_state.ball.position.angleWith(OwnRobot[robot_num].state().position);
        Common::Angle newToRobot = toRobot - tagret_angle;
        Common::Angle deltaAngle = Common::Angle::fromDeg(std::min(std::fabs(newToRobot.deg()), 30.0f));
        newToRobot.setDeg(std::max(0.0f, std::fabs(newToRobot.deg()) - deltaAngle.deg()) *
                          Common::sign(newToRobot.deg()));
        newToRobot = newToRobot + tagret_angle;

        OwnRobot[robot_num].face(m_world_state.ball.position);
        OwnRobot[robot_num].target.angle +=
            (newToRobot + Common::Angle::fromDeg(180.0f) - OwnRobot[robot_num].target.angle) / 2.0f;
        Common::Vec2 target_point =
            m_world_state.ball.position.circleAroundPoint(newToRobot, near_ball_dis / deltaAngle.cos());
        if (near_dis_override > 0)
            navigate(robot_num, target_point, VelocityProfile::aroom());
        else
            navigate(robot_num, target_point, VelocityProfile::mamooli());

        if (OwnRobot[robot_num].state().position.distanceTo(m_world_state.ball.position) > far_ball_dis * shmit_coeff)
        {
            state         = kFar;
            last_change_t = timer.time();
        }

        if (std::fabs(deltaAngle.deg()) < near_angle_tol)
        {
            hys_bank[0]++;
        }
        else
        {
            hys_bank[0] = 0;
        }

        if ((hys_bank[0] > near_to_kick_hys) && ((shoot_pow > 0) || (chip_pow > 0)))
        {
            state         = kKick;
            last_change_t = timer.time();
        }
    }

    else if (state == kKick)
    {
        Common::logDebug("STEPPPPP4");
        if (chip_pow > 0)
        {
            chip_head = OwnRobot[robot_num].state().angle;
        }
        OwnRobot[robot_num].target.angle = tagret_angle + Common::Angle::fromDeg(180.0f);
        navigate(robot_num, m_world_state.ball.position, VelocityProfile::aroom());
        if (shoot_pow > 0)
            OwnRobot[robot_num].shoot(shoot_pow);
        if (chip_pow > 0)
            OwnRobot[robot_num].chip(chip_pow);

        if (OwnRobot[robot_num].state().position.distanceTo(m_world_state.ball.position) > near_ball_dis * shmit_coeff)
        {
            state         = kFar;
            last_change_t = timer.time();
        }
    }
}
} // namespace Tyr::Soccer
