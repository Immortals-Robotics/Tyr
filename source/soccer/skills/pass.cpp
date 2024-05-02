#include "../ai.h"

namespace Tyr::Soccer
{
Common::Vec2 Ai::CalculatePassPos(int robot_num, const Common::Vec2 &target, const Common::Vec2 &statPos, float bar)
{
    Common::Line ball_line = Common::Line::fromPointAndAngle(ball.position, ball.velocity.toAngle());
    // Common::Line ball_line ( 1.0 , -ballLine.getSlope() , -ballLine.getIntercept() );

    if (chip_head.deg() < 180)
    {
        ball_line = Common::Line::fromPointAndAngle(ball.position, chip_head);
        std::cout << "	calcing with static head: " << chip_head.deg() << std::endl;
    }

    Common::Angle angleWithTarget = statPos.angleWith(target);
    // angleWithTarget = OwnRobot[robot_num].State.angle;
    Common::Vec2 ans  = ball_line.closestPoint(statPos + angleWithTarget.toUnitVec() * bar);
    Common::Vec2 fans = ans - angleWithTarget.toUnitVec() * bar;
    Common::debug().drawCircle(fans, 90, "", Common::Cornflower_Blue);
    return fans;
}

float getCalibratedShootPowCPY(int vision_id, float raw_shoot)
{
    if (raw_shoot <= 0)
    {
        return 0;
    }
    vision_id = std::min(11, std::max(0, vision_id));

    static float poly_coeff[Common::Setting::kMaxRobots][3] = {
        {-0.1012, 6.8441, -27.822}, //  0
        {-0.0988, 6.5467, -28.173}, //  1
        {-0.1012, 6.8645, -28.968}, //  2
        {-0.09, 6.2632, -22.909},   //  3
        {-0.0789, 4.8728, -18.189}, //  4
        {-0.0859, 6.2805, -27.016}, //  5
        {-0.0857, 6.2963, -25.36},  //  6
        {-0.0813, 6.1029, -21.593}, //  7
        {-0.0813, 6.1029, -21.593}, //  8*
        {-0.0813, 6.1029, -21.593}, //  9*
        {-0.0813, 6.1029, -21.593}, // 10*
        {-0.0813, 6.1029, -21.593}, // 11*
    };

    float a = poly_coeff[vision_id][0];
    float b = poly_coeff[vision_id][1];
    float c = poly_coeff[vision_id][2];

    raw_shoot = std::min(150.0f, std::max(0.0f, raw_shoot));

    float delta = b * b - 4.0f * a * (c - raw_shoot);
    if (delta < 0)
        delta = 0;

    float calib_shoot = (-b + sqrt(delta)) / (2.0f * a);

    calib_shoot = std::min(100.0f, std::max(0.0f, calib_shoot));

    return calib_shoot;
}

void Ai::WaitForPass(int robot_num, bool chip, Common::Vec2 *target, Common::Vec2 *statPos)
{
    Common::Vec2 pos = CalculatePassPos(robot_num, target == nullptr ? Common::Vec2(-side * Common::worldState().field.width, 0) : *target,
                                        statPos == nullptr ? OwnRobot[robot_num].State.position : *statPos, 78);

    /*if (target==nullptr) {
        target = new Common::Vec2(Common::Vec2(-side*3025, 0));
    }*/

    if (target == nullptr)
    {
        OwnRobot[robot_num].target.angle = calculateOneTouchAngle(robot_num, pos);
    }
    else
    {
        OwnRobot[robot_num].target.angle = OwnRobot[robot_num].State.position.angleWith(*target);
    }

    Common::Line shoot_line =
        Common::Line::fromPointAndAngle(Common::Vec2(pos.x, pos.y), OwnRobot[robot_num].target.angle);
    Common::Line open_line =
        Common::Line::fromPointAndAngle(Common::Vec2(pos.x, pos.y), calculateOpenAngleToGoal(pos, robot_num).center);
    debugDraw = true;
    Common::debug().drawLineSegment(pos, Common::Vec2(-side * Common::worldState().field.width, shoot_line.y(-side * Common::worldState().field.width)), "",
                                    Common::Brown);
    Common::debug().drawLineSegment(pos, Common::Vec2(-side * Common::worldState().field.width, open_line.y(-side * Common::worldState().field.width)), "",
                                    Common::Pink);
    debugDraw = false;

    // OwnRobot[robot_num].target.angle=-90;

    ERRTSetObstacles(robot_num, 0, 1);
    ERRTNavigate2Point(robot_num, pos, 0, 100, &VELOCITY_PROFILE_KHARAKI);

    if (target == nullptr)
    {
        if (chip)
        {
            OwnRobot[robot_num].Chip(60);
        }
        else
        {
            float vel_delta = ball.velocity.length() / 100.0f;
            // vel_delta = std::min(60,vel_delta);
            vel_delta *= 0.7;
            vel_delta = 60 - vel_delta;

            //            if(vel_delta < 44){
            //                vel_delta = 44;
            //            }
            //            vel_delta = getCalibratedShootPowCPY(robot_num,60);
            std::cout << "ball vel: " << vel_delta << std::endl;
            OwnRobot[robot_num].Shoot(40);
        }
    }
    else
    {
        if (chip)
        {
            OwnRobot[robot_num].Chip(0);
        }
        else
            OwnRobot[robot_num].Shoot(0);
    }

    // OwnRobot[robot_num].Dribble( 15 );
}
} // namespace Tyr::Soccer
