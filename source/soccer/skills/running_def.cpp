#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::runningDef(int robot_num, Common::Vec2 target, Common::Vec2 *defendTarget, bool stop)
{
    const float max_def_move_to_intercept = 400.0f;
    const float max_near_opp_dis          = 750.0f;
    const float max_ball_handler_opp_dis  = 400.0f;
    const float max_shoot_blocker_dis     = 1500.0f;
    const float shoot_blocker_r           = 200.0f;
    const float max_fast_ball_spd         = 5000.0f;
    const float max_own_attacker_dis      = 400.0f;

    bool ballAriving      = false;
    bool oneTouchNear     = false;
    bool interceptNear    = false;
    bool oppNear          = false;
    bool oppHasBall       = false;
    bool oppGoalOpen      = false;
    bool ballMovingFast   = false;
    bool ownAttackHasBall = false;
    bool ballIsToGoal     = false;

    ballAriving = oneTouchDetector[robot_num].IsArriving(40, 80);

    Common::Vec2 oneTouchPos = CalculatePassPos(robot_num, oppGoal(), OwnRobot[robot_num].state().position);

    if (oneTouchPos.distanceTo(target) < max_def_move_to_intercept)
    {
        oneTouchNear = true;
    }

    if (m_world_state.ball.position.distanceTo(target) < max_def_move_to_intercept)
    {
        interceptNear = true;
    }

    int nearestOpp = findNearestOpp(target, -1, true);
    oppNear        = m_world_state.opp_robot[nearestOpp].position.distanceTo(target) < max_near_opp_dis;

    int ballHandlerOpp = findNearestOpp(m_world_state.ball.position, -1, true);
    oppHasBall         = m_world_state.opp_robot[ballHandlerOpp].position.distanceTo(m_world_state.ball.position) <
                 max_ball_handler_opp_dis;

    //    ballMovingFast = m_world_state.ball.velocity.length() > max_fast_ball_spd;

    ownAttackHasBall = OwnRobot[attack].state().position.distanceTo(m_world_state.ball.position) < max_own_attacker_dis;
    if (OwnRobot[attack].state().seen_state == Common::SeenState::CompletelyOut)
    {
        ownAttackHasBall = false;
    }

    ballIsToGoal = ballIsGoaling();

    Common::logDebug("interceptNear:{}", interceptNear);
    Common::logDebug("oppHasBall: {}", oppHasBall);
    Common::logDebug("ballMovingFast: {}", ballMovingFast);
    Common::logDebug("ownAttackHasBall: {}", ownAttackHasBall);
    Common::logDebug("ballIsToGoal: {}", ballIsToGoal);
    Common::logDebug("gkIntercepting: {}", gkIntercepting);

    if ((interceptNear) && (!oppHasBall) && (!ballMovingFast) && (!ownAttackHasBall) && (!ballIsToGoal) &&
        (!gkIntercepting) && (!stop))
    //    if(1)
    {
        Common::logDebug("IIIIIIIIIIIIIIIIIIJJJJJJJJJJJJJJJJJJJJJ");
        attacker(robot_num,
                    m_world_state.ball.position.angleWith(Common::Vec2(side * (Common::field().width + 110), 0)), 0, 80,
                    0, 0, 0);
    }
    else
    {
        OwnRobot[robot_num].face(*defendTarget);
        navigate(robot_num, target, VelocityProfile::mamooli());
    }
}
} // namespace Tyr::Soccer
