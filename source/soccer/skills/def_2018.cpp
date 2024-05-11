#include "../ai.h"

#define DEF_VEL_PROFILE (stop ? VelocityProfile::Type::Aroom : VelocityProfile::Type::Mamooli)

namespace Tyr::Soccer
{
void Ai::runningDef(int robot_num, Common::Vec2 target, Common::Vec2 *defendTarget, bool stop)
{
    const float max_def_move_to_intercept    = 400.0f;
    const float max_near_asshole_dis         = 750.0f;
    const float max_ball_handler_asshole_dis = 400.0f;
    const float max_shoot_blocker_dis        = 1500.0f;
    const float shoot_blocker_r              = 200.0f;
    const float max_fast_ball_spd            = 5000.0f;
    const float max_own_attacker_dis         = 400.0f;

    bool ballAriving      = false;
    bool oneTouchNear     = false;
    bool interceptNear    = false;
    bool assholeNear      = false;
    bool assholeHasBall   = false;
    bool oppGoalOpen      = false;
    bool ballMovingFast   = false;
    bool ownAttackHasBall = false;
    bool ballIsToGoal     = false;

    ballAriving = oneTouchDetector[robot_num].IsArriving(40, 80);

    Common::Vec2 oneTouchPos = CalculatePassPos(robot_num, Common::Vec2(-side * Common::field().width, 0),
                                                OwnRobot[robot_num].state().position);

    if (oneTouchPos.distanceTo(target) < max_def_move_to_intercept)
    {
        oneTouchNear = true;
    }

    if (Common::worldState().ball.position.distanceTo(target) < max_def_move_to_intercept)
    {
        interceptNear = true;
    }

    int nearestAsshole = findNearestAsshole(target, -1, true);
    assholeNear = Common::worldState().opp_robot[nearestAsshole].position.distanceTo(target) < max_near_asshole_dis;

    int ballHandlerAsshole = findNearestAsshole(Common::worldState().ball.position, -1, true);
    assholeHasBall         = Common::worldState().opp_robot[ballHandlerAsshole].position.distanceTo(
                         Common::worldState().ball.position) < max_ball_handler_asshole_dis;

    //    ballMovingFast = Common::worldState().ball.velocity.length() > max_fast_ball_spd;

    ownAttackHasBall =
        OwnRobot[attack].state().position.distanceTo(Common::worldState().ball.position) < max_own_attacker_dis;
    if (OwnRobot[attack].state().seen_state == Common::SeenState::CompletelyOut)
    {
        ownAttackHasBall = false;
    }

    ballIsToGoal = ballIsGoaling();

    Common::logDebug("interceptNear:{}", interceptNear);
    Common::logDebug("assholeHasBall: {}", assholeHasBall);
    Common::logDebug("ballMovingFast: {}", ballMovingFast);
    Common::logDebug("ownAttackHasBall: {}", ownAttackHasBall);
    Common::logDebug("ballIsToGoal: {}", ballIsToGoal);
    Common::logDebug("gkIntercepting: {}", gkIntercepting);

    if ((interceptNear) && (!assholeHasBall) && (!ballMovingFast) && (!ownAttackHasBall) && (!ballIsToGoal) &&
        (!gkIntercepting) && (!stop))
    //    if(1)
    {
        Common::logDebug("IIIIIIIIIIIIIIIIIIJJJJJJJJJJJJJJJJJJJJJ");
        ERRTSetObstacles(robot_num, 0, 1);
        // tech_circle(robot_num,Common::sign(Common::worldState().ball.position.y)*side*60 ,0,15,false);
        tech_circle(robot_num,
                    Common::worldState().ball.position.angleWith(
                        Common::Vec2(side * (Common::field().width + 110), 0)),
                    0, 80, true, 0, 0, 0);
    }
    else
    {
        ERRTSetObstacles(robot_num, stop, true);
        OwnRobot[robot_num].face(Common::Vec2((*defendTarget).x, (*defendTarget).y));
        ERRTNavigate2Point(robot_num, target, 100, DEF_VEL_PROFILE);
    }
}

void Ai::DefBy1(int thelastdef_num, Common::Vec2 *defendTarget, bool stop)
{
    Common::logDebug("__________IN_DEF_BY_1_____________________________________________");
    if (thelastdef_num != -1)
    {
        Common::logDebug("1111111111111111111111111111111111111111111111111111111111");
        Common::Angle alpha =
            Common::Vec2(side * Common::field().width, 0).angleWith(Common::worldState().ball.position) +
            Common::Angle::fromDeg(90 + side * 90);
        alpha.setDeg(std::clamp(alpha.deg(), -90.0f, 90.0f));
        float alphaSgn = Common::sign(alpha.deg());

        if (std::fabs(alpha.deg()) < 45.0)
        {

            if (Common::worldState().ball.velocity.length() < 200)
            {
                Common::Line GOAL_LINE =
                    Common::Line::fromTwoPoints(Common::Vec2(side * Common::field().width, 100.0),
                                                Common::Vec2(side * Common::field().width, -100.0));

                Common::Vec2 TARGET_BALL_IN_GOAL = GOAL_LINE.closestPoint(Common::worldState().ball.position);

                if (TARGET_BALL_IN_GOAL.y > 200)
                {
                    TARGET_BALL_IN_GOAL.y = 200.0;
                }
                if (TARGET_BALL_IN_GOAL.y < -200)
                {
                    TARGET_BALL_IN_GOAL.y = -200.0;
                }
                Common::logDebug("TARGET_BALL_IN_GOAL.y: {}", TARGET_BALL_IN_GOAL.y);
                Common::Line ball_line =
                    Common::Line::fromTwoPoints(Common::worldState().ball.position, TARGET_BALL_IN_GOAL);
                Common::Line Front_line = Common::Line::fromPointAndAngle(
                    Common::Vec2(
                        side * (Common::field().width - Common::field().penalty_area_depth - 100),
                        0),
                    Common::Angle::fromDeg(90.0f));
                Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
                Common::Vec2 target = Common::Vec2(ans.x, ans.y);

                runningDef(thelastdef_num, target, defendTarget, stop);
            }
            else
            {
                Common::Line ball_line  = Common::Line::fromPointAndAngle(Common::worldState().ball.position,
                                                                          Common::worldState().ball.velocity.toAngle());
                Common::Line Front_line = Common::Line::fromPointAndAngle(
                    Common::Vec2(
                        side * (Common::field().width - Common::field().penalty_area_depth - 100),
                        0),
                    Common::Angle::fromDeg(90.0f));
                Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
                Common::Vec2 target = Common::Vec2(ans.x, ans.y);

                runningDef(thelastdef_num, target, defendTarget, stop);
            }
        }
        else if (alpha.deg() > 85.0)
        {
            Common::Line ball_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * Common::field().width, 0.0), Common::Angle::fromDeg(85.0f));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0f));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[thelastdef_num].target.angle = Common::Angle::fromDeg(85.0 + 90 + side * 90);
            ERRTSetObstacles(thelastdef_num, stop, 1);
            ERRTNavigate2Point(thelastdef_num, fans, 100, DEF_VEL_PROFILE);
        }
        else if (alpha.deg() >= 45.0)
        {
            Common::Line ball_line = Common::Line::fromTwoPoints(
                Common::worldState().ball.position, Common::Vec2(side * Common::field().width, 0.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 target = Common::Vec2(ans.x, ans.y);

            runningDef(thelastdef_num, target, defendTarget, stop);
        }
        else if (alpha.deg() < -85.0)
        {
            Common::Line ball_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * Common::field().width, 0.0), Common::Angle::fromDeg(-85.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[thelastdef_num].target.angle = Common::Angle::fromDeg(-85.0 + 90 + side * 90);
            ERRTSetObstacles(thelastdef_num, stop, 1);
            ERRTNavigate2Point(thelastdef_num, fans, 100, DEF_VEL_PROFILE);
        }
        else if (alpha.deg() <= -45.0)
        {
            Common::Line ball_line = Common::Line::fromTwoPoints(
                Common::worldState().ball.position, Common::Vec2(side * Common::field().width, 0.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 target = Common::Vec2(ans.x, ans.y);

            runningDef(thelastdef_num, target, defendTarget, stop);
        }
    }
}

void Ai::DefBy2(int rightdef_num, int leftdef_num, Common::Vec2 *defendTarget, bool stop)
{
    Common::Angle alpha =
        Common::Vec2(side * Common::field().width, 0).angleWith(Common::worldState().ball.position) +
        Common::Angle::fromDeg((90 + side * 90));
    alpha.setDeg(std::clamp(alpha.deg(), -90.0f, 90.0f));
    float alphaSgn = Common::sign(alpha.deg());

    if (rightdef_num != -1)
    {
        // rightdef_num
        if (alpha.deg() < -85.0)
        {
            Common::Line ball_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * Common::field().width, 0.0), Common::Angle::fromDeg(-85.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[rightdef_num].target.angle = Common::Angle::fromDeg(-85.0 + 90 + side * 90);
            ERRTSetObstacles(rightdef_num, stop, 1);
            ERRTNavigate2Point(rightdef_num, fans, 100, DEF_VEL_PROFILE);
        }
        else if (alpha.deg() < -48.0)
        {
            Common::Line ball_line = Common::Line::fromTwoPoints(
                Common::worldState().ball.position, Common::Vec2(side * Common::field().width, 0.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 target = Common::Vec2(ans.x, ans.y);

            runningDef(rightdef_num, target, defendTarget, stop);
        }
        else if (alpha.deg() < -3.5)
        {
            if (Common::worldState().ball.velocity.length() < 200)
            {
                Common::Line GOAL_LINE =
                    Common::Line::fromTwoPoints(Common::Vec2(side * Common::field().width, 100.0),
                                                Common::Vec2(side * Common::field().width, -100.0));

                Common::Vec2 TARGET_BALL_IN_GOAL = GOAL_LINE.closestPoint(Common::worldState().ball.position);

                if (TARGET_BALL_IN_GOAL.y > 200)
                {
                    TARGET_BALL_IN_GOAL.y = 200.0;
                }
                if (TARGET_BALL_IN_GOAL.y < -200)
                {
                    TARGET_BALL_IN_GOAL.y = -200.0;
                }

                Common::logDebug("TARGET_BALL_IN_GOAL.y: {}", TARGET_BALL_IN_GOAL.y);
                Common::Line ball_line =
                    Common::Line::fromTwoPoints(Common::worldState().ball.position, TARGET_BALL_IN_GOAL);
                Common::Line Front_line = Common::Line::fromPointAndAngle(
                    Common::Vec2(
                        side * (Common::field().width - Common::field().penalty_area_depth - 100),
                        0),
                    Common::Angle::fromDeg(90.0));
                Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
                Common::Vec2 target = Common::Vec2(ans.x, ans.y);

                runningDef(rightdef_num, target, defendTarget, stop);
            }
            else
            {
                Common::Line ball_line  = Common::Line::fromPointAndAngle(Common::worldState().ball.position,
                                                                          Common::worldState().ball.velocity.toAngle());
                Common::Line Front_line = Common::Line::fromPointAndAngle(
                    Common::Vec2(
                        side * (Common::field().width - Common::field().penalty_area_depth - 100),
                        0),
                    Common::Angle::fromDeg(90.0));
                Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
                Common::Vec2 target = Common::Vec2(ans.x, ans.y);

                runningDef(rightdef_num, target, defendTarget, stop);
            }
        }
        else
        {
            Common::Line ball_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * Common::field().width, 0.0), Common::Angle::fromDeg(-3.5));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(
                    side * (Common::field().width - Common::field().penalty_area_depth - 100), 0),
                Common::Angle::fromDeg(90.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[rightdef_num].target.angle = Common::Angle::fromDeg(90 + side * 90);
            ERRTSetObstacles(rightdef_num, stop, 1);
            ERRTNavigate2Point(rightdef_num, fans, 100, DEF_VEL_PROFILE);
        }
    }

    if (leftdef_num != -1)
    {
        // leftdef_num
        if (alpha.deg() > 85.0)
        {
            Common::Line ball_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * Common::field().width, 0.0), Common::Angle::fromDeg(85.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[leftdef_num].target.angle = Common::Angle::fromDeg(85.0 + 90 + side * 90);
            ERRTSetObstacles(leftdef_num, stop, 1);
            ERRTNavigate2Point(leftdef_num, fans, 100, DEF_VEL_PROFILE);
        }
        else if (alpha.deg() > 48.0)
        {
            Common::Line ball_line = Common::Line::fromTwoPoints(
                Common::worldState().ball.position, Common::Vec2(side * Common::field().width, 0.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 target = Common::Vec2(ans.x, ans.y);

            runningDef(leftdef_num, target, defendTarget, stop);
        }
        else if (alpha.deg() > 3.5)
        {
            //            Common::Line ball_line = Common::Line::fromTwoPoints(Common::worldState().ball.position,
            //                                                         Common::Vec2(side *
            //                                                         Common::field().width, 0.0));
            //            Common::Line Front_line = Common::Line::fromPointAndAngle(
            //                    Common::Vec2(side * (Common::field().width -
            //                    Common::field().penalty_area_depth - 100), 0), 90.0);
            //            Common::Vec2 ans = ball_line.intersect(Front_line).value_or(Common::Vec2());
            //            Common::Vec2 target = Common::Vec2(ans.x, ans.y);
            Common::Line GOAL_LINE =
                Common::Line::fromTwoPoints(Common::Vec2(side * Common::field().width, 100.0),
                                            Common::Vec2(side * Common::field().width, -100.0));

            Common::Vec2 TARGET_BALL_IN_GOAL = GOAL_LINE.closestPoint(Common::worldState().ball.position);

            if (TARGET_BALL_IN_GOAL.y > 300)
            {
                TARGET_BALL_IN_GOAL.y = 300.0;
            }
            if (TARGET_BALL_IN_GOAL.y < -300)
            {
                TARGET_BALL_IN_GOAL.y = -300.0;
            }

            Common::Line ball_line =
                Common::Line::fromTwoPoints(Common::worldState().ball.position, TARGET_BALL_IN_GOAL);
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(
                    side * (Common::field().width - Common::field().penalty_area_depth - 100), 0),
                Common::Angle::fromDeg(90.0));
            Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 target = Common::Vec2(ans.x, ans.y);

            runningDef(leftdef_num, target, defendTarget, stop);
        }
        else
        {
            Common::Line ball_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * Common::field().width, 0.0), Common::Angle::fromDeg(3.5));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(
                    side * (Common::field().width - Common::field().penalty_area_depth - 100), 0),
                Common::Angle::fromDeg(90.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[leftdef_num].target.angle = Common::Angle::fromDeg(90 + side * 90);
            ERRTSetObstacles(leftdef_num, stop, 1);
            ERRTNavigate2Point(leftdef_num, fans, 100, DEF_VEL_PROFILE);
        }
    }
}

void Ai::DefBy3(int middef_num, int rightdef_num, int leftdef_num, Common::Vec2 *defendTarget, bool stop)
{
    Common::Angle alpha =
        Common::Vec2(side * Common::field().width, 0).angleWith(Common::worldState().ball.position) +
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
            //            Common::Line ball_line = Common::Line::fromTwoPoints(Common::worldState().ball.position,
            //                                                         Common::Vec2(side *
            //                                                         Common::field().width, 0.0));
            //            Common::Vec2 ans = ball_line.intersect(Front_line).value_or(Common::Vec2());
            //            Common::Vec2 target = Common::Vec2(ans.x, ans.y);
            Common::Line GOAL_LINE =
                Common::Line::fromTwoPoints(Common::Vec2(side * Common::field().width, 100.0),
                                            Common::Vec2(side * Common::field().width, -100.0));

            Common::Vec2 TARGET_BALL_IN_GOAL = GOAL_LINE.closestPoint(Common::worldState().ball.position);

            if (TARGET_BALL_IN_GOAL.y > 300)
            {
                TARGET_BALL_IN_GOAL.y = 300.0;
            }
            if (TARGET_BALL_IN_GOAL.y < -300)
            {
                TARGET_BALL_IN_GOAL.y = -300.0;
            }

            Common::Line ball_line =
                Common::Line::fromTwoPoints(Common::worldState().ball.position, TARGET_BALL_IN_GOAL);
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(
                    side * (Common::field().width - Common::field().penalty_area_depth - 100), 0),
                Common::Angle::fromDeg(90.0));
            Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 target = Common::Vec2(ans.x, ans.y);

            runningDef(middef_num, target, defendTarget, stop);
        }
        else
        {
            Common::Line ball_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * Common::field().width, 0.0), Common::Angle::fromDeg(alphaSgn * 43.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(
                    side * (Common::field().width - Common::field().penalty_area_depth - 100), 0),
                Common::Angle::fromDeg(90.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[middef_num].target.angle = Common::Angle::fromDeg(alphaSgn * 43.0 + 90 + side * 90);
            ERRTSetObstacles(middef_num, stop, 1);
            ERRTNavigate2Point(middef_num, fans, 100, DEF_VEL_PROFILE);
        }
    }

    if (rightdef_num != -1)
    {
        // rightdef_num
        if (alpha.deg() < -85.0)
        {
            Common::Line ball_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * Common::field().width, 0.0), Common::Angle::fromDeg(-85.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[rightdef_num].target.angle = Common::Angle::fromDeg(-85.0 + 90 + side * 90);
            ERRTSetObstacles(rightdef_num, stop, 1);
            ERRTNavigate2Point(rightdef_num, fans, 100, DEF_VEL_PROFILE);
        }
        else if (alpha.deg() < -48.0)
        {
            Common::Line ball_line = Common::Line::fromTwoPoints(
                Common::worldState().ball.position, Common::Vec2(side * Common::field().width, 0.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 target = Common::Vec2(ans.x, ans.y);

            runningDef(rightdef_num, target, defendTarget, stop);
        }
        else
        {
            Common::Line ball_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * Common::field().width, 0.0), Common::Angle::fromDeg(-48.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[rightdef_num].target.angle = Common::Angle::fromDeg(-48.0 + 90 + side * 90);
            ERRTSetObstacles(rightdef_num, stop, 1);
            ERRTNavigate2Point(rightdef_num, fans, 100, DEF_VEL_PROFILE);
        }
    }

    if (leftdef_num != -1)
    {
        // leftdef_num
        if (alpha.deg() > 85.0)
        {
            Common::Line ball_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * Common::field().width, 0.0), Common::Angle::fromDeg(85.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[leftdef_num].target.angle = Common::Angle::fromDeg(85.0 + 90 + side * 90);
            ERRTSetObstacles(leftdef_num, stop, 1);
            ERRTNavigate2Point(leftdef_num, fans, 100, DEF_VEL_PROFILE);
        }
        else if (alpha.deg() > 48.0)
        {
            Common::Line ball_line = Common::Line::fromTwoPoints(
                Common::worldState().ball.position, Common::Vec2(side * Common::field().width, 0.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans    = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 target = Common::Vec2(ans.x, ans.y);

            runningDef(leftdef_num, target, defendTarget, stop);
        }
        else
        {
            Common::Line ball_line = Common::Line::fromPointAndAngle(
                Common::Vec2(side * Common::field().width, 0.0), Common::Angle::fromDeg(48.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -side * (Common::field().penalty_area_depth + 100)),
                Common::Angle::fromDeg(0.0));
            Common::Vec2 ans  = ball_line.intersect(Front_line).value_or(Common::Vec2());
            Common::Vec2 fans = Common::Vec2(ans.x, ans.y);

            OwnRobot[leftdef_num].target.angle = Common::Angle::fromDeg(48.0 + 90 + side * 90);
            ERRTSetObstacles(leftdef_num, stop, 1);
            ERRTNavigate2Point(leftdef_num, fans, 100, DEF_VEL_PROFILE);
        }
    }
}

void Ai::DefMid(int &middef_num, int &rightdef_num, int &leftdef_num, Common::Vec2 *defendTarget, bool stop,
                bool replace)
{

    Common::Angle alpha =
        Common::Vec2(side * Common::field().width, 0).angleWith(Common::worldState().ball.position);
    alpha.setDeg(std::clamp(alpha.deg(), -90.0f, 90.0f));
    float alphaSgn = Common::sign(alpha.deg());
    Common::logDebug("ALPHA: {}", alpha);

    if (!defendTarget)
        defendTarget = &(Common::worldState().ball.position);

    // make sure the def is present:
    if (OwnRobot[middef_num].state().seen_state == Common::SeenState::CompletelyOut && replace)
    {
        if (OwnRobot[rightdef_num].state().seen_state != Common::SeenState::CompletelyOut)
        {
            std::swap(middef_num, rightdef_num);
        }
        else if (OwnRobot[leftdef_num].state().seen_state != Common::SeenState::CompletelyOut)
        {
            std::swap(middef_num, leftdef_num);
        }
        else
        { // We're busted...
        }
    }

    bool leftdef_available =
        OwnRobot[leftdef_num].state().seen_state != Common::SeenState::CompletelyOut && markMap[&leftdef_num] == -1;
    bool rightdef_available =
        OwnRobot[rightdef_num].state().seen_state != Common::SeenState::CompletelyOut && markMap[&leftdef_num] == -1;

    DefHi(middef_num, defendTarget, stop);

    if (!leftdef_available && !rightdef_available)
    {
        DefBy1(middef_num, defendTarget, stop);
    }
    else if (!rightdef_available)
    {
        DefBy2(middef_num, leftdef_num, defendTarget, stop);
    }
    else if (!leftdef_available)
    {
        DefBy2(rightdef_num, middef_num, defendTarget, stop);
    }
    else
    {
        DefBy3(middef_num, rightdef_num, leftdef_num, defendTarget, stop);
    }
}
} // namespace Tyr::Soccer
