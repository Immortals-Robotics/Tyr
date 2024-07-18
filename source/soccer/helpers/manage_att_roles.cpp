#include "../ai.h"

namespace Tyr::Soccer
{
static int             attackerChangeHys = 0;
static constexpr float kChangeHysSeconds = 0.5f;

void Ai::manageAttRoles()
{
    const int required_hys_frames = kChangeHysSeconds * Common::config().vision.vision_frame_rate;

    int *best_attack =
        *std::max_element(m_ids.begin(), m_ids.end(), [&](int *lhs, int *rhs)
                          { return calculateSwitchToAttackerScore(*lhs) < calculateSwitchToAttackerScore(*rhs); });

    const float current_attack_score = calculateSwitchToAttackerScore(m_attack);
    const float best_attack_score    = calculateSwitchToAttackerScore(*best_attack);

    // < 0 means the current robot should be switched immediately, ie. out of the game
    if (current_attack_score < 0.f && best_attack_score >= 0.0f)
    {
        attackerChangeHys = required_hys_frames + 1;
    }
    else if (*best_attack != m_attack && best_attack_score - current_attack_score > 0.1f)
    {
        attackerChangeHys++;
    }

    if (attackerChangeHys > required_hys_frames)
    {
        std::swap(m_attack, *best_attack);
        attackerChangeHys = 0;
    }
}
} // namespace Tyr::Soccer
