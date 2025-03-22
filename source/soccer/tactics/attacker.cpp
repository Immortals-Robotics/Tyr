#include "attacker.h"

#include "../robot/robot.h"

#define OLD_ATTACKER 0

#if OLD_ATTACKER
#include "../skills/old_attacker.h"
#else
#include "../skills/intercept_ball.h"
#endif

namespace Tyr::Soccer
{
const Tactic::Id AttackerTactic::kId = &AttackerTactic::kId;

void AttackerTactic::execute(Robot &t_robot)
{
#if OLD_ATTACKER
    OldAttackerSkill{m_angle, m_kick, m_chip, m_kiss, m_dribbler, m_precise, m_is_gk}.execute(t_robot);
#else
    InterceptBallSkill{m_angle}.execute(t_robot);
#endif
}

NavigationFlags AttackerTactic::getNavigationFlags(const Robot &t_robot) const
{
    (void) m_kick;
    (void) m_chip;
    
    NavigationFlags navigation_flags = NavigationFlags::None;

    if (State::ref().running() && t_robot.state().position.distanceTo(State::world().ball.position) < 500.0f)
    {
        navigation_flags |= NavigationFlags::NoExtraMargin;
        navigation_flags |= NavigationFlags::NoBreak;
    }

    return navigation_flags;
}
} // namespace Tyr::Soccer
