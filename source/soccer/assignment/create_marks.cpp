#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::createMidAssignments()
{
    std::vector<std::pair<int, float>> crunchingOpps;

    if (m_is_defending)
    {
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
    }

    int marker_idx = 0;
    for (auto &mark_map_entry : m_mark_map)
    {
        int *const role         = mark_map_entry.first;
        const bool actual_robot = (role == &m_mid1 || role == &m_mid2 || role == &m_mid5);

        if (marker_idx < crunchingOpps.size())
        {
            const int opp_idx     = crunchingOpps[marker_idx].first;
            mark_map_entry.second = opp_idx;

            Assignment assignment{};

            assignment.role          = mark_map_entry.first;
            assignment.priority      = actual_robot ? Assignment::Priority::Medium : Assignment::Priority::None;
            assignment.cost_function = std::bind(&Ai::markRoleCost, this, std::placeholders::_1, std::placeholders::_2);
            assignment.target_idx    = crunchingOpps[marker_idx].first;

            m_assignments.push_back(assignment);
        }
        else
        {
            mark_map_entry.second = -1;

            createStaticAssignment(mark_map_entry.first,
                                   actual_robot ? Assignment::Priority::Low : Assignment::Priority::None);
        }

        ++marker_idx;
    }
}
} // namespace Tyr::Soccer
