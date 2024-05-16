#include "../ai.h"

namespace Tyr::Soccer
{
Common::Vec2 Ai::CalculatePassPos(int robot_num, const Common::Vec2 &target, const Common::Vec2 &statPos, float bar)
{
    Common::Line ball_line =
        Common::Line::fromPointAndAngle(m_world_state.ball.position, m_world_state.ball.velocity.toAngle());
    // Common::Line ball_line ( 1.0 , -ballLine.getSlope() , -ballLine.getIntercept() );

    if (chip_head.deg() < 180)
    {
        ball_line = Common::Line::fromPointAndAngle(m_world_state.ball.position, chip_head);
        Common::logDebug("calcing with static head: {}", chip_head);
    }

    Common::Angle angleWithTarget = statPos.angleWith(target);
    // angleWithTarget = OwnRobot[robot_num].state().angle;
    Common::Vec2 ans  = ball_line.closestPoint(statPos + angleWithTarget.toUnitVec() * bar);
    Common::Vec2 fans = ans - angleWithTarget.toUnitVec() * bar;
    Common::debug().draw(fans, Common::Color::dark_blue());
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

void Ai::WaitForPass(int robot_num, bool chip, const Common::Vec2 *target, Common::Vec2 *statPos)
{
    Common::Vec2 pos =
        CalculatePassPos(robot_num, target == nullptr ? Common::Vec2(-side * Common::field().width, 0) : *target,
                         statPos == nullptr ? OwnRobot[robot_num].state().position : *statPos, 78);

    /*if (target==nullptr) {
        target = new Common::Vec2(Common::Vec2(-side*3025, 0));
    }*/

    if (target == nullptr)
    {
        OwnRobot[robot_num].target.angle = calculateOneTouchAngle(robot_num, pos);
    }
    else
    {
        OwnRobot[robot_num].target.angle = OwnRobot[robot_num].state().position.angleWith(*target);
    }

    Common::Line shoot_line =
        Common::Line::fromPointAndAngle(Common::Vec2(pos.x, pos.y), OwnRobot[robot_num].target.angle);
    Common::Line open_line =
        Common::Line::fromPointAndAngle(Common::Vec2(pos.x, pos.y), calculateOpenAngleToGoal(pos, robot_num).center);
    Common::debug().draw(Common::LineSegment{pos, Common::Vec2(-side * Common::field().width,
                                                               shoot_line.y(-side * Common::field().width))},
                         Common::Color::brown());
    Common::debug().draw(Common::LineSegment{pos, Common::Vec2(-side * Common::field().width,
                                                               open_line.y(-side * Common::field().width))},
                         Common::Color::pink());

    // OwnRobot[robot_num].target.angle=-90;

    ERRTSetObstacles(robot_num);
    ERRTNavigate2Point(robot_num, pos, 100, VelocityProfile::Type::Kharaki);

    if (target == nullptr)
    {
        if (chip)
        {
            OwnRobot[robot_num].chip(60);
        }
        else
        {
            float vel_delta = m_world_state.ball.velocity.length() / 100.0f;
            // vel_delta = std::min(60,vel_delta);
            vel_delta *= 0.7;
            vel_delta = 60 - vel_delta;

            //            if(vel_delta < 44){
            //                vel_delta = 44;
            //            }
            //            vel_delta = getCalibratedShootPowCPY(robot_num,60);
            Common::logDebug("ball vel: {}", vel_delta);
            OwnRobot[robot_num].shoot(40);
        }
    }
    else
    {
        if (chip)
        {
            OwnRobot[robot_num].chip(0);
        }
        else
            OwnRobot[robot_num].shoot(0);
    }

    // OwnRobot[robot_num].dribble( 15 );
}
} // namespace Tyr::Soccer
