#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::staticZoneScore(Zone &t_zone) const
{
    t_zone.best_pos = t_zone.rect.center();
    t_zone.score    = -1.0f;

    // Skip if it contains a penalty area
    if (ownPenaltyArea().inside(t_zone.rect.center()) || oppPenaltyArea().inside(t_zone.rect.center()))
        return;

    // Skip the zone if the ball is inside it
    if (t_zone.rect.inside(m_world_state.ball.position))
    {
        t_zone.score = 0.0f;
        return;
    }

    int robots_inside = 0;
    for (const auto &robot : m_world_state.own_robot)
    {
        if (robot.seen_state == Common::SeenState::CompletelyOut)
            continue;

        if (robot.vision_id != m_attack && robot.vision_id != m_def1 && robot.vision_id != m_def2)
            continue;

        if (t_zone.rect.inside(robot.position))
            ++robots_inside;
    }

    // Skip too many of our robots if it is inside the zone
    if (robots_inside > 2)
    {
        t_zone.score = 0.0f;
        return;
    }

    // TODO: find the best position in the zone
    t_zone.best_pos = t_zone.rect.center();
    t_zone.score    = m_is_defending ? staticPosScoreDefence(t_zone.best_pos) : staticPosScoreAttack(t_zone.best_pos);
}
} // namespace Tyr::Soccer
