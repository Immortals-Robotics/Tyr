#include "../ai.h"

#include "../helpers/ball_is_goaling.h"
#include "../helpers/find_nearest_opp.h"

#include "../tactics/attacker.h"

namespace Tyr::Soccer
{
void Ai::runningDef(Robot &t_robot, const Common::Vec2 t_target, Common::Vec2 *t_defend_target)
{
    const float max_def_move_to_intercept = 400.0f;
    const float max_ball_handler_opp_dis  = 400.0f;
    const float max_own_attacker_dis      = 400.0f;

    bool interceptNear    = false;
    bool oppHasBall       = false;
    bool ballMovingFast   = false;
    bool ownAttackHasBall = false;
    bool ballIsToGoal     = false;

    if (m_world_state.ball.position.distanceTo(t_target) < max_def_move_to_intercept)
    {
        interceptNear = true;
    }

    const std::optional<Common::RobotState> ballHandlerOpp = findNearestOpp(m_world_state.ball.position, -1, true);
    oppHasBall                                             = ballHandlerOpp.has_value() &&
                 ballHandlerOpp.value().position.distanceTo(m_world_state.ball.position) < max_ball_handler_opp_dis;

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

    if ((interceptNear) && (!oppHasBall) && (!ballMovingFast) && (!ownAttackHasBall) && (!ballIsToGoal) &&
        m_ref_state.running())
    //    if(1)
    {
        Common::logDebug("IIIIIIIIIIIIIIIIIIJJJJJJJJJJJJJJJJJJJJJ");
        AttackerTactic{m_world_state.ball.position.angleWith(Common::Vec2(m_side * (Common::field().width + 110), 0)),
                       0, 80, 0, 0}
            .execute(t_robot);
    }
    else
    {
        t_robot.face(*t_defend_target);
        t_robot.navigate(t_target, VelocityProfile::mamooli());
    }
}
} // namespace Tyr::Soccer
