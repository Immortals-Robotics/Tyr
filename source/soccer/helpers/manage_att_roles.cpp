#include "../ai.h"

namespace Tyr::Soccer
{
static int           attackerChangeHys = 0;
static constexpr int kRequiredHys      = 30;

void Ai::manageAttRoles()
{
    // Check if the current attacker has lost the ball, before switching its role
    if (m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position) > 300)
        attackerChangeHys++;
    if (m_own_robot[m_attack].state().seen_state == Common::SeenState::CompletelyOut)
        attackerChangeHys = kRequiredHys + 1;

    if (attackerChangeHys > kRequiredHys)
    {
        int *best_attack =
            *std::max_element(ids.begin(), ids.end(), [&](int *lhs, int *rhs)
                              { return calculateSwitchToAttackerScore(*lhs) < calculateSwitchToAttackerScore(*rhs); });

        if (calculateSwitchToAttackerScore(*best_attack) > 0.1f)
        {
            std::swap(m_attack, *best_attack);
            attackerChangeHys = 0;
        }
    }

    if (m_own_robot[m_mid1].state().position.y < m_own_robot[m_mid2].state().position.y)
    {
        std::swap(m_mid1, m_mid2);
    }
}
} // namespace Tyr::Soccer
