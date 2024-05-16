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

void Ai::WaitForPass(int robot_num, bool chip, const Common::Vec2 *target, Common::Vec2 *statPos)
{
    Common::Vec2 pos = CalculatePassPos(robot_num, target == nullptr ? oppGoal() : *target,
                                        statPos == nullptr ? OwnRobot[robot_num].state().position : *statPos, 78);

    if (target == nullptr)
    {
        OwnRobot[robot_num].target.angle = calculateOneTouchAngle(robot_num, pos);
    }
    else
    {
        OwnRobot[robot_num].target.angle = OwnRobot[robot_num].state().position.angleWith(*target);
    }

    const OpenAngle open_angle = calculateOpenAngleToGoal(pos, robot_num);

    Common::Line shoot_line = Common::Line::fromPointAndAngle(pos, OwnRobot[robot_num].target.angle);
    Common::Line open_line  = Common::Line::fromPointAndAngle(pos, open_angle.center);
    Common::debug().draw(Common::LineSegment{pos, Common::Vec2(oppGoal().x, shoot_line.y(oppGoal().x))},
                         Common::Color::brown());
    Common::debug().draw(Common::LineSegment{pos, Common::Vec2(oppGoal().x, open_line.y(oppGoal().x))},
                         Common::Color::pink());

    setObstacles(robot_num);
    navigate(robot_num, pos, VelocityProfile::kharaki());

    if (target == nullptr)
    {
        if (chip)
        {
            OwnRobot[robot_num].chip(60);
        }
        else
        {
            float vel_delta = m_world_state.ball.velocity.length() / 100.0f;
            vel_delta *= 0.7;
            vel_delta = 60 - vel_delta;
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
}
} // namespace Tyr::Soccer
