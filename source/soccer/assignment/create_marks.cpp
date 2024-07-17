#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::createMarkAssignments()
{
    if (!m_is_defending)
    {
        for (auto &pair : m_mark_map)
        {
            pair.second = -1;
        }
        return;
    }

    std::vector<std::pair<int, float>> crunchingOpps;

    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        float threat = calculateOppThreat(i, m_ref_state.theirRestart());
        if (threat < 0)
            continue;
        crunchingOpps.push_back(std::make_pair(i, threat));
    }

    std::sort(crunchingOpps.begin(), crunchingOpps.end(),
              [](const std::pair<int, float> &a, const std::pair<int, float> &b) -> bool
              { return a.second > b.second; });

    Common::logDebug("Opps: {}", crunchingOpps.size());
    for (auto it = crunchingOpps.begin(); it != crunchingOpps.end(); ++it)
    {
        Common::logDebug("-- {} : {}", it->first, it->second);
    }

    int marker_idx = 0;
    for (auto &mark_map_entry : m_mark_map)
    {
        Assignment assignment{};

        assignment.role = mark_map_entry.first;

        if (marker_idx < crunchingOpps.size())
        {
            const int opp_idx     = crunchingOpps[marker_idx].first;
            mark_map_entry.second = opp_idx;

            assignment.priority      = Assignment::Priority::Medium;
            assignment.cost_function = std::bind(&Ai::markRoleCost, this, std::placeholders::_1, std::placeholders::_2);
            assignment.target_idx    = crunchingOpps[marker_idx].first;
        }
        else
        {
            mark_map_entry.second = -1;
            assignment.priority   = Assignment::Priority::Low;
            assignment.cost_function =
                std::bind(&Ai::staticRoleCost, this, std::placeholders::_1, std::placeholders::_2);
            // TODO: static position for markers
            assignment.target_point = Common::Vec2{};
        }

        m_assignments.push_back(assignment);
        ++marker_idx;
    }
}
} // namespace Tyr::Soccer
