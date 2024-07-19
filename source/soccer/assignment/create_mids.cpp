#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::createMidAssignments()
{
    std::vector<std::pair<int, float>> crunchingOpps;

    const bool state_needs_marks = m_ref_state.stop() || m_ref_state.running() || m_ref_state.theirFreeKick();
    if (m_is_defending && state_needs_marks)
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

    // Find the number of available mids

    int available_robots = 0;
    for (const Common::RobotState &robot : m_world_state.own_robot)
    {
        if (robot.seen_state != Common::SeenState::CompletelyOut)
            ++available_robots;
    }

    // this assumes that we always have exactly 4 non-mid robots
    // gk, def1, def2, attack
    const int mid_robots = available_robots - 4;

    for (int mid_idx = 0; mid_idx < m_prioritized_mids.size(); ++mid_idx)
    {
        int *const role = m_prioritized_mids[mid_idx];

        if (mid_idx >= mid_robots)
        {
            m_mark_map[role] = -1;
            createStaticAssignment(role, Assignment::Priority::None);
            continue;
        }

        if (mid_idx < crunchingOpps.size())
        {
            const int opp_idx = crunchingOpps[mid_idx].first;
            m_mark_map[role]  = opp_idx;

            Assignment assignment{};

            assignment.role          = role;
            assignment.priority      = Assignment::Priority::Medium;
            assignment.cost_function = std::bind(&Ai::markRoleCost, this, std::placeholders::_1, std::placeholders::_2);
            assignment.target_idx    = opp_idx;

            m_assignments.push_back(assignment);
        }
        else
        {
            m_mark_map[role] = -1;

            const bool shoot = !m_is_defending;
            const bool chip  = !m_is_defending && role == &m_mid5;

            createStaticAssignment(role, Assignment::Priority::Low, shoot, chip);
        }
    }
}
} // namespace Tyr::Soccer
