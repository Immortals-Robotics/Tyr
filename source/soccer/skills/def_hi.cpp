#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::defHi(int t_robot_num, int t_right_def_num, int t_left_def_num, Common::Vec2 *t_defend_target, bool t_stop)
{
    if (!t_defend_target)
        t_defend_target = &(m_world_state.ball.position);

    Common::Angle alpha = ownGoal().angleWith(m_world_state.ball.position) + Common::Angle::fromDeg(90 + m_side * 90);
    alpha.setDeg(std::clamp(alpha.deg(), -90.0f, 90.0f));
    float alphaSgn = Common::sign(alpha.deg());

    if (t_robot_num != -1)
    {
        if (std::fabs(alpha.deg()) < 43.0)
        {
            Common::Line GOAL_LINE = Common::Line::fromSegment(ownGoalLine());

            Common::Vec2 TARGET_BALL_IN_GOAL = GOAL_LINE.closestPoint(m_world_state.ball.position);

            if (TARGET_BALL_IN_GOAL.y > 300)
            {
                TARGET_BALL_IN_GOAL.y = 300.0;
            }
            if (TARGET_BALL_IN_GOAL.y < -300)
            {
                TARGET_BALL_IN_GOAL.y = -300.0;
            }

            Common::Line ball_line  = Common::Line::fromTwoPoints(m_world_state.ball.position, TARGET_BALL_IN_GOAL);
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(m_side * (Common::field().width - Common::field().penalty_area_depth - 100), 0),
                Common::Angle::fromDeg(90.0));
            Common::Vec2 target = ball_line.intersect(Front_line).value_or(Common::Vec2());

            runningDef(t_robot_num, target, t_defend_target, t_stop);
        }
        else
        {
            Common::Line ball_line =
                Common::Line::fromPointAndAngle(ownGoal(), Common::Angle::fromDeg(alphaSgn * 43.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(m_side * (Common::field().width - Common::field().penalty_area_depth - 100), 0),
                Common::Angle::fromDeg(90.0));
            Common::Vec2 fans = ball_line.intersect(Front_line).value_or(Common::Vec2());

            m_own_robot[t_robot_num].target.angle = Common::Angle::fromDeg(alphaSgn * 43.0 + 90 + m_side * 90);
            navigate(t_robot_num, fans, VelocityProfile::mamooli());
        }
    }

    if (t_right_def_num != -1)
    {
        // t_right_def_num
        if (alpha.deg() < -85.0)
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(ownGoal(), Common::Angle::fromDeg(-85.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 fans = ball_line.intersect(Front_line).value_or(Common::Vec2());

            m_own_robot[t_right_def_num].target.angle = Common::Angle::fromDeg(-85.0 + 90 + m_side * 90);
            navigate(t_right_def_num, fans, VelocityProfile::mamooli());
        }
        else if (alpha.deg() < -48.0)
        {
            Common::Line ball_line  = Common::Line::fromTwoPoints(m_world_state.ball.position, ownGoal());
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 target = ball_line.intersect(Front_line).value_or(Common::Vec2());

            runningDef(t_right_def_num, target, t_defend_target, t_stop);
        }
        else
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(ownGoal(), Common::Angle::fromDeg(-48.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 fans = ball_line.intersect(Front_line).value_or(Common::Vec2());

            m_own_robot[t_right_def_num].target.angle = Common::Angle::fromDeg(-48.0 + 90 + m_side * 90);
            navigate(t_right_def_num, fans, VelocityProfile::mamooli());
        }
    }

    if (t_left_def_num != -1)
    {
        // t_left_def_num
        if (alpha.deg() > 85.0)
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(ownGoal(), Common::Angle::fromDeg(85.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 fans = ball_line.intersect(Front_line).value_or(Common::Vec2());

            m_own_robot[t_left_def_num].target.angle = Common::Angle::fromDeg(85.0 + 90 + m_side * 90);
            navigate(t_left_def_num, fans, VelocityProfile::mamooli());
        }
        else if (alpha.deg() > 48.0)
        {
            Common::Line ball_line  = Common::Line::fromTwoPoints(m_world_state.ball.position, ownGoal());
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 target = ball_line.intersect(Front_line).value_or(Common::Vec2());

            runningDef(t_left_def_num, target, t_defend_target, t_stop);
        }
        else
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(ownGoal(), Common::Angle::fromDeg(48.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 fans = ball_line.intersect(Front_line).value_or(Common::Vec2());

            m_own_robot[t_left_def_num].target.angle = Common::Angle::fromDeg(48.0 + 90 + m_side * 90);
            navigate(t_left_def_num, fans, VelocityProfile::mamooli());
        }
    }
}
} // namespace Tyr::Soccer
