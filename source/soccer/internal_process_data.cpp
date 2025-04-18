#include "ai.h"

namespace Tyr::Soccer
{
void Ai::internalProcessData()
{
    for (Robot &robot : m_own_robot)
    {
        robot.reset();
    }

    if (m_ref_state.our_side == Common::TeamSide::Right)
        m_side = 1;
    else
        m_side = -1;

    if (m_ref_state.stop() || m_ref_state.halt())
        m_allaf_pos.clear();

    for (auto &zone : m_zones)
    {
        staticZoneScore(zone);
    }

    m_sorted_zones.clear();
    for (const auto &zone : m_zones)
    {
        if (zone.score < 0)
            continue;

        m_sorted_zones.push_back(&zone);

        if (!Common::almostEqual(zone.score, 0.0f))
        {
            Common::Color color = Common::Color::lerp(Common::Color::black(), Common::Color::blue(), zone.score);
            color.a             = 0.5f;

            Common::debug().draw(zone.rect, color, true);
        }
    }

    std::sort(m_sorted_zones.begin(), m_sorted_zones.end(),
              [](const Zone *a, const Zone *b) -> bool { return a->score > b->score; });
    Common::logInfo("Found {} valid zones", m_sorted_zones.size());
}
} // namespace Tyr::Soccer
