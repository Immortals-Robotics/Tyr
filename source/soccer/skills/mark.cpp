#include "../ai.h"

#include "mark.h"

#include "mark_to_ball.h"
#include "mark_to_goal.h"
#include "wait_for_pass.h"

namespace Tyr::Soccer
{
const Skill::Id MarkSkill::kId = &MarkSkill::kId;

void MarkSkill::execute(Robot &t_robot)
{
    (void)t_robot;
    /*const Common::Vec2 oppToBall = (State::world().ball.position - m_opp.position).normalized();
    const Common::Vec2 oppToGoal = (State::ownGoal() - m_opp.position).normalized();

    const float oppToGoalDis = m_opp.position.distanceTo(State::ownGoal());
    const float oneTouchDot  = oppToBall.dot(oppToGoal);

    if (oneTouchDot > 0 || oppToGoalDis < 2500)
    {
        mark2Goal(t_robot, m_opp, m_dist, Common::config().soccer.penalty_area_mark);
    }
    else
    {
        if (t_robot.one_touch_detector.isArriving(20))
        {
            waitForPass(t_robot, false);
        }
        else
        {
            mark2Ball(t_robot, m_opp, m_dist);
        }
    }*/
}

void Ai::mark(Robot &t_robot, const Common::RobotState &t_opp, const float t_dist)
{
    const Common::Vec2 oppToBall = (m_world_state.ball.position - t_opp.position).normalized();
    const Common::Vec2 oppToGoal = (ownGoal() - t_opp.position).normalized();

    const float oppToGoalDis = t_opp.position.distanceTo(ownGoal());
    const float oneTouchDot  = oppToBall.dot(oppToGoal);

    if (oneTouchDot > 0 || oppToGoalDis < 2500)
    {
        MarkToGoalSkill{t_opp, t_dist, Common::config().soccer.penalty_area_mark}.execute(t_robot);
    }
    else
    {
        if (t_robot.one_touch_detector.isArriving(20))
        {
            WaitForPassSkill{}.execute(t_robot);
        }
        else
        {
            MarkToBallSkill{t_opp, t_dist}.execute(t_robot);
        }
    }
}
} // namespace Tyr::Soccer
