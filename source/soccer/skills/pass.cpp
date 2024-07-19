#include "../ai.h"

namespace Tyr::Soccer
{
Common::Vec2 Ai::calculatePassPos(const Common::Vec2 &t_target, const Common::Vec2 &t_stat_pos, const float t_bar)
{
    Common::Line ball_line =
        Common::Line::fromPointAndAngle(m_world_state.ball.position, m_world_state.ball.velocity.toAngle());
    // Common::Line ball_line ( 1.0 , -m_ball_line.getSlope() , -m_ball_line.getIntercept() );

    // TODO: this is broken
#if 0
    if (m_chip_head.deg() < 180)
    {
        ball_line = Common::Line::fromPointAndAngle(m_world_state.ball.position, m_chip_head);
        Common::logDebug("calcing with static head: {}", m_chip_head);
    }
#endif

    Common::Angle angleWithTarget = t_stat_pos.angleWith(t_target);
    // angleWithTarget = m_own_robot[t_robot_num].state().angle;
    Common::Vec2 ans  = ball_line.closestPoint(t_stat_pos + angleWithTarget.toUnitVec() * t_bar);
    Common::Vec2 fans = ans - angleWithTarget.toUnitVec() * t_bar;
    Common::debug().draw(fans, Common::Color::darkBlue());
    return fans;
}

void Ai::waitForPass(const int t_robot_num, const bool t_chip, const Common::Vec2 *t_target, Common::Vec2 *t_stat_pos)
{
    Common::Vec2 pos =
        calculatePassPos(t_target == nullptr ? oppGoal() : *t_target,
                         t_stat_pos == nullptr ? m_own_robot[t_robot_num].state().position : *t_stat_pos, 78);

    if (t_target == nullptr)
    {
        m_own_robot[t_robot_num].target.angle = calculateOneTouchAngle(t_robot_num, pos);
    }
    else
    {
        m_own_robot[t_robot_num].target.angle = m_own_robot[t_robot_num].state().position.angleWith(*t_target);
    }

    const OpenAngle open_angle = calculateOpenAngleToGoal(pos, t_robot_num);

    Common::Line shoot_line = Common::Line::fromPointAndAngle(pos, m_own_robot[t_robot_num].target.angle);
    Common::Line open_line  = Common::Line::fromPointAndAngle(pos, open_angle.center);
    Common::debug().draw(Common::LineSegment{pos, Common::Vec2(oppGoal().x, shoot_line.y(oppGoal().x))},
                         Common::Color::brown());
    Common::debug().draw(Common::LineSegment{pos, Common::Vec2(oppGoal().x, open_line.y(oppGoal().x))},
                         Common::Color::pink());

    navigate(t_robot_num, pos, VelocityProfile::kharaki());

    if (t_target == nullptr)
    {
        if (t_chip)
        {
            m_own_robot[t_robot_num].chip(60);
        }
        else
        {
            float vel_delta = m_world_state.ball.velocity.length() / 100.0f;
            vel_delta *= 0.7f;
            vel_delta = 60 - vel_delta;
            Common::logDebug("ball vel: {}", vel_delta);
            //TODO: calc pass speed
            m_own_robot[t_robot_num].shoot(4000.f);
        }
    }
    else
    {
        if (t_chip)
        {
            m_own_robot[t_robot_num].chip(0);
        }
        else
            m_own_robot[t_robot_num].shoot(0.f);
    }
}
} // namespace Tyr::Soccer
