#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::DefHi(int robot_num, Common::Vec2 *defendTarget, bool stop)
{
    // side = -side;
    if (!defendTarget)
        defendTarget = &(ball.Position);

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

    Common::Vec2 target = DefGhuz(defendTarget);

    ballAriving = oneTouchDetector[robot_num].IsArriving(40, 80);

    Common::Vec2 oneTouchPos =
        CalculatePassPos(robot_num, Common::Vec2(-side * field_width, 0), OwnRobot[robot_num].State.Position);

    if (oneTouchPos.distanceTo(target) < max_def_move_to_intercept)
    {
        oneTouchNear = true;
    }

    if (ball.Position.distanceTo(target) < max_def_move_to_intercept)
    {
        interceptNear = true;
    }

    int nearestAsshole = findNearestAsshole(target, -1, true);
    assholeNear        = OppRobot[nearestAsshole].Position.distanceTo(target) < max_near_asshole_dis;

    int ballHandlerAsshole = findNearestAsshole(ball.Position, -1, true);
    assholeHasBall = OppRobot[ballHandlerAsshole].Position.distanceTo(ball.Position) < max_ball_handler_asshole_dis;

    oppGoalOpen = true;
    Common::Line ballGoalLine =
        Common::Line::fromTwoPoints(Common::Vec2(-field_width * side, 0), Common::Vec2(target.x, target.y));
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OppRobot[i].seenState == Common::CompletelyOut)
            continue;
        if ((std::fabs(OppRobot[i].Position.x) > field_width) || (std::fabs(OppRobot[i].Position.y) > field_height))
            continue;
        if (OppRobot[i].Position.distanceTo(target) > max_shoot_blocker_dis)
            continue;

        if (ballGoalLine.distanceTo(OppRobot[i].Position) < shoot_blocker_r)
        {
            oppGoalOpen = false;
            break;
        }
    }
    if (oppGoalOpen)
    {
        for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
        {
            if (OwnRobot[i].State.seenState == Common::CompletelyOut)
                continue;
            if ((std::fabs(OwnRobot[i].State.Position.x) > field_width) ||
                (std::fabs(OwnRobot[i].State.Position.y) > field_height))
                continue;
            if (OwnRobot[i].State.Position.distanceTo(target) > max_shoot_blocker_dis)
                continue;

            if (ballGoalLine.distanceTo(Common::Vec2(OwnRobot[i].State.Position.x, OwnRobot[i].State.Position.y)) <
                shoot_blocker_r)
            {
                oppGoalOpen = false;
                break;
            }
        }
    }

    ballMovingFast = ball.velocity.length() > max_fast_ball_spd;

    ownAttackHasBall = OwnRobot[attack].State.Position.distanceTo(ball.Position) < max_own_attacker_dis;
    if (OwnRobot[attack].State.seenState == Common::CompletelyOut)
    {
        ownAttackHasBall = false;
    }

    ballIsToGoal = ballIsGoaling();
    ballIsToGoal = false;

    //	std::cout << "interceptNear:	"<<interceptNear <<std::endl;
    //    std::cout << "assholeHasBall: " << assholeHasBall<<std::endl;
    //    std::cout << "ballMovingFast: " << ballMovingFast <<std::endl;
    //    std::cout<<"ownAttackHasBall: " << ownAttackHasBall<<std::endl;
    //    std::cout<< "ballMovingFast: " << ballMovingFast <<std::endl;
    //    std::cout<< "ownAttackHasBall: " << ownAttackHasBall <<std::endl;
    //    std::cout<< "ballIsToGoal: " << ballIsToGoal << std::endl;
    //    std::cout<<"gkIntercepting: "<< gkIntercepting << std::endl;

    if ((false) && (ballAriving) && (oneTouchNear) && (!assholeNear) && (!assholeHasBall) && (!ballMovingFast) &&
        (!ownAttackHasBall) && (!ballIsToGoal))
    {
        WaitForPass(robot_num, !oppGoalOpen);
    }
    else if ((interceptNear) && (!assholeHasBall) && (!ballMovingFast) && (!ownAttackHasBall) && (!ballIsToGoal) &&
             (!gkIntercepting) && (!stop))
    {
        ERRTSetObstacles(robot_num, 0, 1);
        // tech_circle(robot_num,Common::sign(ball.Position.y)*side*60 ,0,15,false);
        tech_circle(robot_num, ball.Position.angleWith(Common::Vec2(side * (field_width + 110), 0)), 0, 500, true, 0, 0,
                    0);
    }
    else
    {
        ERRTSetObstacles(robot_num, stop, true);
        OwnRobot[robot_num].face(Common::Vec2((*defendTarget).x, (*defendTarget).y));
        ERRTNavigate2Point(robot_num, target, 0, 100, stop ? &VELOCITY_PROFILE_AROOM : &VELOCITY_PROFILE_MAMOOLI);
    }
    // side = -side;
}
} // namespace Tyr::Soccer