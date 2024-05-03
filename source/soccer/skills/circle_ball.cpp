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
        std::cout << "STEPPPPP1" << std::endl;
        OwnRobot[robot_num].face(Common::worldState().ball.position);
        ERRTSetObstacles(robot_num, 0, 1);
        ERRTNavigate2Point(robot_num, Common::worldState().ball.position, 1, 30, &VELOCITY_PROFILE_MAMOOLI);

        Common::debug().drawCircle(Common::worldState().ball.position, very_far_ball_dis - 90.0f, "", Common::Red);

        if (OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position) < very_far_ball_dis)
        {
            state         = kFar;
            last_change_t = timer.time();
        }
    }
    else if (state == kFar)
    {
        std::cout << "STEPPPPP2" << std::endl;
        OwnRobot[robot_num].face(Common::worldState().ball.position);
        ERRTSetObstacles(robot_num, 0, 1);
        Common::Vec2 target_point =
            Common::worldState().ball.position.circleAroundPoint(Common::worldState().ball.position.angleWith(OwnRobot[robot_num].State.position), near_ball_dis);
        ERRTNavigate2Point(robot_num, target_point, 1, 20, &VELOCITY_PROFILE_AROOM);

        Common::debug().drawCircle(Common::worldState().ball.position, far_ball_dis - 90.0f, "", Common::Pink);

        if (OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position) < far_ball_dis)
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
        else if (OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position) > very_far_ball_dis * shmit_coeff)
        {
            state         = kVeryFar;
            last_change_t = timer.time();
        }
    }
    else if (state == kNear)
    {
        std::cout << "STEPPPPP3" << std::endl;
        Common::Angle toRobot    = Common::worldState().ball.position.angleWith(OwnRobot[robot_num].State.position);
        Common::Angle newToRobot = toRobot - tagret_angle;
        Common::Angle deltaAngle = Common::Angle::fromDeg(std::min(std::fabs(newToRobot.deg()), 30.0f));
        newToRobot.setDeg(std::max(0.0f, std::fabs(newToRobot.deg()) - deltaAngle.deg()) *
                          Common::sign(newToRobot.deg()));
        newToRobot = newToRobot + tagret_angle;

        OwnRobot[robot_num].face(Common::worldState().ball.position);
        OwnRobot[robot_num].target.angle +=
            (newToRobot + Common::Angle::fromDeg(180.0f) - OwnRobot[robot_num].target.angle) / 2.0f;
        // OwnRobot[robot_num].target.angle = NormalizeAngle(OwnRobot[robot_num].target.angle);
        ERRTSetObstacles(robot_num, 0, 1);
        Common::Vec2 target_point = Common::worldState().ball.position.circleAroundPoint(newToRobot, near_ball_dis / deltaAngle.cos());
        if (near_dis_override > 0)
            ERRTNavigate2Point(robot_num, target_point, 1, 20, &VELOCITY_PROFILE_AROOM);
        else
            ERRTNavigate2Point(robot_num, target_point, 1, 20, &VELOCITY_PROFILE_MAMOOLI);

        if (OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position) > far_ball_dis * shmit_coeff)
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
        std::cout << "STEPPPPP4" << std::endl;
        if (chip_pow > 0)
        {
            chip_head = OwnRobot[robot_num].State.angle;
        }
        // OwnRobot[robot_num].face(Common::worldState().ball.position);
        OwnRobot[robot_num].target.angle = tagret_angle + Common::Angle::fromDeg(180.0f);
        ERRTSetObstacles(robot_num, 0, 1);
        ERRTNavigate2Point(robot_num, Common::worldState().ball.position, 1, 100, &VELOCITY_PROFILE_AROOM);
        if (shoot_pow > 0)
            OwnRobot[robot_num].Shoot(shoot_pow);
        if (chip_pow > 0)
            OwnRobot[robot_num].Chip(chip_pow);
        Common::debug().drawCircle(Common::worldState().ball.position, very_far_ball_dis - 90.0f, "", Common::Red);
        // tech_circle(robot_num, tagret_angle, shoot_pow, chip_pow, 1, 1, 0, 0);

        if (OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position) > near_ball_dis * shmit_coeff)
        {
            state         = kFar;
            last_change_t = timer.time();
        }
    }

    Common::debug().drawLineSegment(OwnRobot[robot_num].State.position, OwnRobot[robot_num].target.position, "",
                                    Common::Black);
}

void Ai::circle_ball_free(int robot_num, Common::Angle tagret_angle, int shoot_pow, int chip_pow, float precision,
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
        OwnRobot[robot_num].face(Common::worldState().ball.position);
        ERRTSetObstacles(robot_num, 0, 0);
        ERRTNavigate2Point(robot_num, Common::worldState().ball.position, 1, 30, &VELOCITY_PROFILE_AROOM);

        Common::debug().drawCircle(Common::worldState().ball.position, very_far_ball_dis - 90.0f, "", Common::Red);

        if (OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position) < very_far_ball_dis)
        {
            state         = kFar;
            last_change_t = timer.time();
        }
    }
    else if (state == kFar)
    {
        OwnRobot[robot_num].face(Common::worldState().ball.position);
        ERRTSetObstacles(robot_num, 0, 0);
        Common::Vec2 target_point =
            Common::worldState().ball.position.circleAroundPoint(Common::worldState().ball.position.angleWith(OwnRobot[robot_num].State.position), near_ball_dis);
        ERRTNavigate2Point(robot_num, target_point, 1, 20, &VELOCITY_PROFILE_AROOM);

        Common::debug().drawCircle(Common::worldState().ball.position, far_ball_dis - 90.0f, "", Common::Pink);

        if (OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position) < far_ball_dis)
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
        else if (OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position) > very_far_ball_dis * shmit_coeff)
        {
            state         = kVeryFar;
            last_change_t = timer.time();
        }
    }
    else if (state == kNear)
    {
        Common::Angle toRobot    = Common::worldState().ball.position.angleWith(OwnRobot[robot_num].State.position);
        Common::Angle newToRobot = toRobot - tagret_angle;
        Common::Angle deltaAngle = Common::Angle::fromDeg(std::min(std::fabs(newToRobot.deg()), 30.0f));
        newToRobot.setDeg(std::max(0.0f, std::fabs(newToRobot.deg()) - deltaAngle.deg()) *
                          Common::sign(newToRobot.deg()));
        newToRobot += tagret_angle;

        OwnRobot[robot_num].face(Common::worldState().ball.position);
        OwnRobot[robot_num].target.angle +=
            (newToRobot + Common::Angle::fromDeg(180.0f) - OwnRobot[robot_num].target.angle) / 2.0f;
        // OwnRobot[robot_num].target.angle = NormalizeAngle(OwnRobot[robot_num].target.angle);
        ERRTSetObstacles(robot_num, 0, 0);
        Common::Vec2 target_point = Common::worldState().ball.position.circleAroundPoint(newToRobot, near_ball_dis / deltaAngle.cos());
        if (near_dis_override > 0)
            ERRTNavigate2Point(robot_num, target_point, 1, 20, &VELOCITY_PROFILE_AROOM);
        else
            ERRTNavigate2Point(robot_num, target_point, 1, 20, &VELOCITY_PROFILE_MAMOOLI);

        if (OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position) > far_ball_dis * shmit_coeff)
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
        if (chip_pow > 0)
        {
            chip_head = OwnRobot[robot_num].State.angle;
        }
        // OwnRobot[robot_num].face(Common::worldState().ball.position);
        OwnRobot[robot_num].target.angle = tagret_angle + Common::Angle::fromDeg(180.0f);
        ERRTSetObstacles(robot_num, 0, 0);
        ERRTNavigate2Point(robot_num, Common::worldState().ball.position, 1, 100, &VELOCITY_PROFILE_AROOM);
        if (shoot_pow > 0)
            OwnRobot[robot_num].Shoot(shoot_pow);
        if (chip_pow > 0)
            OwnRobot[robot_num].Chip(chip_pow);
        Common::debug().drawCircle(Common::worldState().ball.position, very_far_ball_dis - 90.0f, "", Common::Red);
        // tech_circle(robot_num, tagret_angle, shoot_pow, chip_pow, 1, 1, 0, 0);

        if (OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position) > near_ball_dis * shmit_coeff)
        {
            state         = kFar;
            last_change_t = timer.time();
        }
    }

    Common::debug().drawLineSegment(OwnRobot[robot_num].State.position, OwnRobot[robot_num].target.position, "",
                                    Common::Black);
}
} // namespace Tyr::Soccer
