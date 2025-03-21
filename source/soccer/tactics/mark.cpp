#include "mark.h"

#include "../robot/robot.h"

#include "../skills/mark_to_ball.h"
#include "../skills/mark_to_goal.h"
#include "../skills/wait_for_pass.h"

namespace Tyr::Soccer
{
const Tactic::Id MarkTactic::kId = &MarkTactic::kId;

void MarkTactic::execute(Robot &t_robot)
{
    const Common::Vec2 oppToBall = (State::world().ball.position - m_opp.position).normalized();
    const Common::Vec2 oppToGoal = (Field::ownGoal() - m_opp.position).normalized();

    const float oppToGoalDis = m_opp.position.distanceTo(Field::ownGoal());
    const float oneTouchDot  = oppToBall.dot(oppToGoal);

    if (oneTouchDot > 0 || oppToGoalDis < 2500)
    {
        MarkToGoalSkill{m_opp, m_dist, Common::config().soccer.penalty_area_mark}.execute(t_robot);
    }
    else
    {
        if (t_robot.one_touch_detector.isArriving(20))
        {
            WaitForPassSkill{}.execute(t_robot);
        }
        else
        {
            MarkToBallSkill{m_opp, m_dist}.execute(t_robot);
        }
    }
}
} // namespace Tyr::Soccer
