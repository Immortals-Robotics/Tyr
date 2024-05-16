#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::DefHi(int middef_num, int rightdef_num, int leftdef_num, Common::Vec2 *defendTarget, bool stop)
{
    if (!defendTarget)
        defendTarget = &(m_world_state.ball.position);

    Common::Angle alpha = Common::Vec2(side * Common::field().width, 0).angleWith(m_world_state.ball.position) +
                          Common::Angle::fromDeg(90 + side * 90);
    alpha.setDeg(std::clamp(alpha.deg(), -90.0f, 90.0f));
    float alphaSgn = Common::sign(alpha.deg());

    if (middef_num != -1)
    {
        if (std::fabs(alpha.deg()) < 43.0)
        {
            //            Common::Line Front_line = Common::Line::fromPointAndAngle(
            //                    Common::Vec2(side * (Common::field().width -
            //                    Common::field().penalty_area_depth - 100), 0), 90.0);
            //            Common::Line ball_line = Common::Line::fromTwoPoints(m_world_state.ball.position,
            //                                                         Common::Vec2(side *
            //                                                         Common::field().width, 0.0));
            //            Common::Vec2 ans = ball_line.intersect(Front_line).value_or(Common::Vec2());
            //            Common::Vec2 target = Common::Vec2(ans.x, ans.y);
            Common::Line GOAL_LINE = Common::Line::fromTwoPoints(Common::Vec2(side * Common::field().width, 100.0),
                                                                 Common::Vec2(side * Common::field().width, -100.0));

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
                Common::Vec2(side * (Common::field().width - Common::field().penalty_area_depth - 100), 0),
                Common::Angle::fromDeg(90.0));
            Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 target = Common::Vec2(ans.x, ans.y);

            runningDef(middef_num, target, defendTarget, stop);
        }
        else
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(Common::Vec2(side * Common::field().width, 0.0),
                                                                      Common::Angle::fromDeg(alphaSgn * 43.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * (Common::field().width - Common::field().penalty_area_depth - 100), 0),
                Common::Angle::fromDeg(90.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[middef_num].target.angle = Common::Angle::fromDeg(alphaSgn * 43.0 + 90 + side * 90);
            setObstacles(middef_num);
            navigate(middef_num, fans, VelocityProfile::mamooli());
        }
    }

    if (rightdef_num != -1)
    {
        // rightdef_num
        if (alpha.deg() < -85.0)
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(Common::Vec2(side * Common::field().width, 0.0),
                                                                      Common::Angle::fromDeg(-85.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[rightdef_num].target.angle = Common::Angle::fromDeg(-85.0 + 90 + side * 90);
            setObstacles(rightdef_num);
            navigate(rightdef_num, fans, VelocityProfile::mamooli());
        }
        else if (alpha.deg() < -48.0)
        {
            Common::Line ball_line  = Common::Line::fromTwoPoints(m_world_state.ball.position,
                                                                  Common::Vec2(side * Common::field().width, 0.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 target = Common::Vec2(ans.x, ans.y);

            runningDef(rightdef_num, target, defendTarget, stop);
        }
        else
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(Common::Vec2(side * Common::field().width, 0.0),
                                                                      Common::Angle::fromDeg(-48.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[rightdef_num].target.angle = Common::Angle::fromDeg(-48.0 + 90 + side * 90);
            setObstacles(rightdef_num);
            navigate(rightdef_num, fans, VelocityProfile::mamooli());
        }
    }

    if (leftdef_num != -1)
    {
        // leftdef_num
        if (alpha.deg() > 85.0)
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(Common::Vec2(side * Common::field().width, 0.0),
                                                                      Common::Angle::fromDeg(85.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[leftdef_num].target.angle = Common::Angle::fromDeg(85.0 + 90 + side * 90);
            setObstacles(leftdef_num);
            navigate(leftdef_num, fans, VelocityProfile::mamooli());
        }
        else if (alpha.deg() > 48.0)
        {
            Common::Line ball_line  = Common::Line::fromTwoPoints(m_world_state.ball.position,
                                                                  Common::Vec2(side * Common::field().width, 0.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 target = Common::Vec2(ans.x, ans.y);

            runningDef(leftdef_num, target, defendTarget, stop);
        }
        else
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(Common::Vec2(side * Common::field().width, 0.0),
                                                                      Common::Angle::fromDeg(48.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[leftdef_num].target.angle = Common::Angle::fromDeg(48.0 + 90 + side * 90);
            setObstacles(leftdef_num);
            navigate(leftdef_num, fans, VelocityProfile::mamooli());
        }
    }
}
} // namespace Tyr::Soccer
