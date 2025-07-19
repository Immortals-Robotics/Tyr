#include "one_touch.h"

#include "../robot/robot.h"
#include "../helpers/open_angle.h"

namespace Tyr::Soccer
{
const Skill::Id OneTouchSkill::kId = &OneTouchSkill::kId;

void OneTouchSkill::execute(Robot &t_robot)
{
    Common::Vec2 pos = calculatePassPos(m_target == nullptr ? Field::oppGoal() : *m_target,
                                        t_robot.state().position, 78);

    if (m_target == nullptr)
    {
        t_robot.target.angle = calculateOneTouchAngle(t_robot, pos);

        const OpenAngle open_angle = OpenAngle::calculateOpenAngleToGoal(pos, t_robot);

        Common::Line shoot_line = Common::Line::fromPointAndAngle(pos, t_robot.target.angle);
        Common::Line open_line  = Common::Line::fromPointAndAngle(pos, open_angle.center);
        Common::debug().draw(
            Common::LineSegment{pos, Common::Vec2(Field::oppGoal().x, shoot_line.y(Field::oppGoal().x))},
            Common::Color::brown());
        Common::debug().draw(
            Common::LineSegment{pos, Common::Vec2(Field::oppGoal().x, open_line.y(Field::oppGoal().x))},
            Common::Color::pink());
    }
    else
    {
        t_robot.target.angle = t_robot.state().position.angleWith(*m_target);
    }

    t_robot.navigate(pos, VelocityProfile::kharaki());

    const bool angle_correct = true;//std::fabs((t_robot.state().angle - t_robot.target.angle).deg()) < 25.0f;

    if (angle_correct)
    {
        // TODO: calc pass speed
        t_robot.shoot(m_kick);
        t_robot.chip(m_chip);
    }
}

Common::Vec2 OneTouchSkill::calculatePassPos(const Common::Vec2 &t_target, const Common::Vec2 &t_stat_pos,
                                             const float         t_bar)
{
    Common::Line ball_line =
        Common::Line::fromPointAndAngle(State::world().ball.position, State::world().ball.velocity.toAngle());
    // Common::Line ball_line ( 1.0 , -m_ball_line.getSlope() , -m_ball_line.getIntercept() );

    // TODO: this is broken
#if 0
    if (m_chip_head.deg() < 180)
    {
        ball_line = Common::Line::fromPointAndAngle(State::world().ball.position, m_chip_head);
        Common::logDebug("calcing with static head: {}", m_chip_head);
    }
#endif

    Common::Angle angleWithTarget = t_stat_pos.angleWith(t_target);
    // angleWithTarget = t_robot.state().angle;
    Common::Vec2 ans  = ball_line.closestPoint(t_stat_pos + angleWithTarget.toUnitVec() * t_bar);
    Common::Vec2 fans = ans - angleWithTarget.toUnitVec() * t_bar;
    Common::debug().draw(fans, Common::Color::darkBlue());
    return fans;
}

Common::Angle OneTouchSkill::calculateOneTouchAngle(const Robot &t_robot, const Common::Vec2 t_one_touch_position)
{
    // t_one_touch_position = State::world().ball.position;
    float v0x, v0y;
    float v1x, v1y;

    v0x = State::world().ball.velocity.x;
    v0y = State::world().ball.velocity.y;

    const Common::LineSegment targetLine = Field::oppGoalLine();

    OpenAngle boz = OpenAngle::calculateOpenAngleToGoal(t_one_touch_position, t_robot);
    Common::logDebug("Open angle : {}    {}", boz.center.deg(), boz.magnitude.deg());

    Common::Line ball_line = Common::Line::fromPointAndAngle(t_one_touch_position, boz.center);

    Common::Vec2 goal = ball_line.intersect(targetLine).value_or(Common::Vec2(Field::oppGoal().x, 0.0f));

    // TODO: cleanup the following

    float aa  = -Common::sign(goal.x) * 90;
    float max = std::numeric_limits<float>::max();
    float gg  = 0;

    while (aa < 180 - 90 * Common::sign(goal.x))
    {
        float a = (aa / 180.0) * 3.14;
        v1x     = Common::config().soccer.one_touch_beta * (-sin(a) * v0x + cos(a) * v0y) * (-sin(a)) +
                  Common::config().soccer.one_touch_shoot_k * cos(a) +
                  Common::config().soccer.one_touch_gamma * (v0x - 2 * (v0x * cos(a) + v0y * sin(a)) * cos(a));
        v1y = Common::config().soccer.one_touch_beta * (-sin(a) * v0x + cos(a) * v0y) * (cos(a)) +
              Common::config().soccer.one_touch_shoot_k * sin(a) +
              Common::config().soccer.one_touch_gamma * (v0y - 2 * (v0x * cos(a) + v0y * sin(a)) * sin(a));
        float e = v1x * (-t_robot.state().position.y + goal.y) +
                  v1y * (t_robot.state().position.x - goal.x);
        if (std::fabs(e) < max)
        {
            max = std::fabs(e);
            gg  = aa;
        }
        aa++;
    }

    return Common::Angle::fromDeg(gg);
}
} // namespace Tyr::Soccer
