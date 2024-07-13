#include "../ai.h"

namespace Tyr::Soccer
{
static int           attackerChangeHys = 0;
static constexpr int kRequiredHys      = 30;

void Ai::manageAttRoles()
{
    int *best_attack =
        *std::max_element(ids.begin(), ids.end(), [&](int *lhs, int *rhs)
                          { return calculateSwitchToAttackerScore(*lhs) < calculateSwitchToAttackerScore(*rhs); });

    if (calculateSwitchToAttackerScore(m_attack) < 0.f)
    {
        attackerChangeHys = kRequiredHys + 1;
    }
    else if (*best_attack != m_attack)
    {
        attackerChangeHys++;
    }

    if (attackerChangeHys > kRequiredHys)
    {
        std::swap(m_attack, *best_attack);
        attackerChangeHys = 0;
    }

    if (m_own_robot[m_mid1].state().position.y < m_own_robot[m_mid2].state().position.y)
    {
        std::swap(m_mid1, m_mid2);
    }
}
} // namespace Tyr::Soccer
