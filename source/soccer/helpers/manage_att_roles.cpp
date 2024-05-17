#include "../ai.h"

namespace Tyr::Soccer
{
static int attackerChangeHys = 0;

void Ai::manageAttRoles()
{
    // Check if the current attacker has lost the ball, before switching its role
    if (m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position) > 600)
        attackerChangeHys++;
    if (m_own_robot[m_attack].state().seen_state == Common::SeenState::CompletelyOut)
        attackerChangeHys = 100;

    if (attackerChangeHys > 30)
    {
        int newAttack = m_attack;

        auto mid1_score = calculateSwitchToAttackerScore(m_mid1);
        auto mid2_score = calculateSwitchToAttackerScore(m_mid2);
        if (mid1_score > 0.1f && mid2_score > 0.1f)
        {
            if (mid1_score > mid2_score)
                newAttack = m_mid1;
            else
                newAttack = m_mid2;
        }
        else if (mid1_score > 0.1f)
        {
            newAttack = m_mid1;
        }
        else if (mid2_score > 0.1f)
        {
            newAttack = m_mid2;
        }

        if (m_attack != newAttack)
        {
            if (m_mid1 == newAttack)
            {
                m_mid1 = m_attack;
            }
            else if (m_mid2 == newAttack)
            {
                m_mid2 = m_attack;
            }

            attackerChangeHys = 0;
            m_attack          = newAttack;
        }
    }

    if (m_own_robot[m_mid1].state().position.y < m_own_robot[m_mid2].state().position.y)
    {
        std::swap(m_mid1, m_mid2);
    }
}
} // namespace Tyr::Soccer
