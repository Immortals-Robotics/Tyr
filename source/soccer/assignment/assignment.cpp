#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::assignRoles()
{
    assignRolesInternal(Assignment::Priority::Max);
    assignRolesInternal(Assignment::Priority::High);
    assignRolesInternal(Assignment::Priority::Medium);
    assignRolesInternal(Assignment::Priority::Low);

    // Fill unfilled assignments with an unavailable robot
    // to avoid having -1 in any role
    std::queue<int> unavailable_robots;
    for (int idx = 0; idx < Common::Config::Common::kMaxRobots; idx++)
    {
        if (m_world_state.own_robot[idx].seen_state == Common::SeenState::CompletelyOut)
            unavailable_robots.push(idx);
    }

    for (Assignment &assignment : m_assignments)
    {
        if (assignment.new_assignee == -1)
        {
            const int unavailable_robot = unavailable_robots.front();
            unavailable_robots.pop();
            Common::logWarning("Assignment not made for priority {}, assigning unavailable robot {}", (int) assignment.priority, unavailable_robot);

            assignment.new_assignee = unavailable_robot;
        }

        *assignment.role = assignment.new_assignee;
    }
}

void Ai::assignRolesInternal(const Assignment::Priority t_priority)
{
    using namespace lemon;
    DIGRAPH_TYPEDEFS(SmartDigraph);

    SmartDigraph graph;

    // Build the example graph
    IntNodeMap        supply(graph);
    std::vector<Node> source;
    std::vector<Node> dest;

    std::set<int> assigned;
    for (const auto &assignment : m_assignments)
    {
        if (assignment.new_assignee != -1)
        {
            if (assigned.contains(assignment.new_assignee))
                Common::logWarning("Robot {} is assigned to multiple roles", assignment.new_assignee);

            assigned.insert(assignment.new_assignee);
        }
    }

    source.reserve(Common::Config::Common::kMaxRobots);
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        Node u    = graph.addNode();
        supply[u] = 1;
        source.push_back(u);
    }

    std::unordered_map<int, int> dst_to_assignment_map;
    dest.reserve(m_assignments.size());
    for (int i = 0; i < m_assignments.size(); i++)
    {
        const Assignment &assignment = m_assignments[i];

        // Skip assignments that are not of the correct priority
        if (assignment.priority != t_priority)
            continue;

        // Skip assignments that have already been assigned
        if (assignment.new_assignee != -1)
            continue;

        dst_to_assignment_map[dest.size()] = i;

        Node u    = graph.addNode();
        supply[u] = -1;
        dest.push_back(u);
    }

    // Skip if there are no assignments to be made
    if (dest.empty())
        return;

    DoubleArcMap cost_map(graph);

    for (int src_idx = 0; src_idx < source.size(); src_idx++)
    {
        const int robot_idx = src_idx;

        // Skip robots that have already been assigned
        if (assigned.contains(robot_idx))
            continue;

        const Common::RobotState &robot = m_world_state.own_robot[robot_idx];

        // Skip robots that are not seen
        if (robot.seen_state == Common::SeenState::CompletelyOut)
            continue;

        for (int dst_idx = 0; dst_idx < dest.size(); dst_idx++)
        {
            const int         assignment_idx = dst_to_assignment_map[dst_idx];
            const Assignment &assignment     = m_assignments[assignment_idx];

            const float cost = assignment.cost_function(robot_idx, assignment);

            if (cost < 0)
                continue;

            const Node u = source[src_idx];
            const Node v = dest[dst_idx];

            const Arc arc = graph.addArc(u, v);
            cost_map[arc] = cost;
        }
    }

    // Run NetworkSimplex algorithm with "LEQ" supply type
    using NS = NetworkSimplex<SmartDigraph, int, float>;
    NS ns(graph);
    ns.supplyType(NS::LEQ);
    ns.costMap(cost_map).supplyMap(supply);
    NS::ProblemType result = ns.run();

    if (result == NS::OPTIMAL)
    {
        Common::logDebug("Priority {}, optimal flow found, total cost: {}", (int) t_priority, ns.totalCost());
    }
    else
    {
        Common::logWarning("Priority {}, optimal flow not found, total cost: {}", (int) t_priority, ns.totalCost());
    }

    for (ArcIt a(graph); a != INVALID; ++a)
    {
        if (ns.flow(a) > 0)
        {
            const int src_idx = graph.id(graph.source(a));
            const int dst_idx = graph.id(graph.target(a)) - source.size();

            const int   robot_idx      = src_idx;
            const int   assignment_idx = dst_to_assignment_map[dst_idx];
            Assignment &assignment     = m_assignments[assignment_idx];

            const float cost = cost_map[a];

            assignment.new_assignee    = robot_idx;
            assignment.assignment_cost = cost;

            Common::logTrace("Robot: {} (old: {}), Assignment: {}, cost: {}", robot_idx, assignment.currentAssignee(),
                             assignment_idx, cost);
        }
    }
}
} // namespace Tyr::Soccer
