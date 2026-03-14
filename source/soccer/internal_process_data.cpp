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

        if (!Common::almostEqual(zone.attack_score, 0.0f) || !Common::almostEqual(zone.defense_score, 0.0f))
        {
            // red channel = attack score, blue channel = defense score
            // red = attack only, blue = defense only, purple = good for both
            Common::Color att_color = Common::Color::lerp(Common::Color::black(), Common::Color::red(), zone.attack_score);
            Common::Color def_color = Common::Color::lerp(Common::Color::black(), Common::Color::blue(), zone.defense_score);
            Common::Color color     = att_color;
            color.b                 = def_color.b;
            color.a                 = 0.5f;

            Common::debug().draw(zone.rect, color, true);
        }
    }

    std::sort(m_sorted_zones.begin(), m_sorted_zones.end(),
              [](const Zone *a, const Zone *b) -> bool { return a->score > b->score; });
    Common::logInfo("Found {} valid zones", m_sorted_zones.size());
}
} // namespace Tyr::Soccer
