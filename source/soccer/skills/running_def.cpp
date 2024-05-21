#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::runningDef(const int t_robot_num, const Common::Vec2 t_target, Common::Vec2 *t_defend_target)
{
    const float max_def_move_to_intercept = 400.0f;
    const float max_near_opp_dis          = 750.0f;
    const float max_ball_handler_opp_dis  = 400.0f;
    const float max_own_attacker_dis      = 400.0f;

    bool ballAriving      = false;
    bool oneTouchNear     = false;
    bool interceptNear    = false;
    bool oppNear          = false;
    bool oppHasBall       = false;
    bool ballMovingFast   = false;
    bool ownAttackHasBall = false;
    bool ballIsToGoal     = false;

    ballAriving = m_one_touch_detector[t_robot_num].IsArriving(40, 80);

    Common::Vec2 oneTouchPos = calculatePassPos(oppGoal(), m_own_robot[t_robot_num].state().position);

    if (oneTouchPos.distanceTo(t_target) < max_def_move_to_intercept)
    {
        oneTouchNear = true;
    }

    if (m_world_state.ball.position.distanceTo(t_target) < max_def_move_to_intercept)
    {
        interceptNear = true;
    }

    int nearestOpp = findNearestOpp(t_target, -1, true);
    oppNear        = m_world_state.opp_robot[nearestOpp].position.distanceTo(t_target) < max_near_opp_dis;

    int ballHandlerOpp = findNearestOpp(m_world_state.ball.position, -1, true);
    oppHasBall         = m_world_state.opp_robot[ballHandlerOpp].position.distanceTo(m_world_state.ball.position) <
                 max_ball_handler_opp_dis;

    //    ballMovingFast = m_world_state.ball.velocity.length() > max_fast_ball_spd;

    ownAttackHasBall =
        m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position) < max_own_attacker_dis;
    if (m_own_robot[m_attack].state().seen_state == Common::SeenState::CompletelyOut)
    {
        ownAttackHasBall = false;
    }

    ballIsToGoal = ballIsGoaling();

    Common::logDebug("interceptNear:{}", interceptNear);
    Common::logDebug("oppHasBall: {}", oppHasBall);
    Common::logDebug("ballMovingFast: {}", ballMovingFast);
    Common::logDebug("ownAttackHasBall: {}", ownAttackHasBall);
    Common::logDebug("ballIsToGoal: {}", ballIsToGoal);
    Common::logDebug("m_gk_intercepting: {}", m_gk_intercepting);

    if ((interceptNear) && (!oppHasBall) && (!ballMovingFast) && (!ownAttackHasBall) && (!ballIsToGoal) &&
        (!m_gk_intercepting) && m_ref_state.gameOn())
    //    if(1)
    {
        Common::logDebug("IIIIIIIIIIIIIIIIIIJJJJJJJJJJJJJJJJJJJJJ");
        attacker(t_robot_num,
                 m_world_state.ball.position.angleWith(Common::Vec2(m_side * (Common::field().width + 110), 0)), 0, 80,
                 0, 0);
    }
    else
    {
        m_own_robot[t_robot_num].face(*t_defend_target);
        navigate(t_robot_num, t_target, VelocityProfile::mamooli());
    }
}
} // namespace Tyr::Soccer
