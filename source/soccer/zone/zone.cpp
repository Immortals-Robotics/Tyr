#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::staticZoneScore(Zone &t_zone) const
{
    t_zone.best_pos = t_zone.rect.center();
    t_zone.score    = -1.0f;

    // Skip the zone if the ball is inside it
    if (t_zone.rect.inside(m_world_state.ball.position))
        return;

    // Skip if it contains a penalty area
    if (ownPenaltyArea().inside(t_zone.rect.center()) || oppPenaltyArea().inside(t_zone.rect.center()))
        return;

    int robots_inside = 0;
    for (const auto &robot : m_world_state.own_robot)
    {
        if (robot.seen_state == Common::SeenState::CompletelyOut)
            continue;

        if (t_zone.rect.inside(robot.position))
        {
            if (robot.vision_id == m_attack)
                robots_inside += 3;
            else
                ++robots_inside;
        }
    }

    // Skip too many of our robots if it is inside the zone
    if (robots_inside > 2)
        return;

    // TODO: find the best position in the zone
    t_zone.best_pos = t_zone.rect.center();
    t_zone.score    = m_is_defending ? staticPosScoreDefence(t_zone.best_pos) : staticPosScoreAttack(t_zone.best_pos);
}
} // namespace Tyr::Soccer
