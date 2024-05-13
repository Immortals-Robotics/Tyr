#include "../ai.h"

namespace Tyr::Soccer
{
static int attackerChangeHys = 0;

void Ai::ManageAttRoles()
{
    if (OwnRobot[attack].state().position.distanceTo(m_world_state.ball.position) >
        600) // Check if the current attacker has lost the ball, before switching its role
        attackerChangeHys++;
    if (OwnRobot[attack].state().seen_state == Common::SeenState::CompletelyOut)
        attackerChangeHys = 100;

    if (attackerChangeHys > 30)
    {
        int newAttack = attack;

        auto mid1_score = calculateSwitchToAttackerScore(mid1);
        auto mid2_score = calculateSwitchToAttackerScore(mid2);
        if (mid1_score > 0.1f && mid2_score > 0.1f)
        {
            if (mid1_score > mid2_score)
                newAttack = mid1;
            else
                newAttack = mid2;
        }
        else if (mid1_score > 0.1f)
        {
            newAttack = mid1;
        }
        else if (mid2_score > 0.1f)
        {
            newAttack = mid2;
        }

        if (attack != newAttack)
        {
            if (mid1 == newAttack)
            {
                mid1 = attack;
            }
            else if (mid2 == newAttack)
            {
                mid2 = attack;
            }

            attackerChangeHys = 0;
            attack            = newAttack;
        }
    }

    if (OwnRobot[mid1].state().position.y < OwnRobot[mid2].state().position.y)
    {
        std::swap(mid1, mid2);
    }
}
} // namespace Tyr::Soccer
