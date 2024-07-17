#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::assignRoles(std::vector<Assignment> *const t_assignments)
{
    assignRolesInternal(t_assignments, Assignment::Priority::Max);
    assignRolesInternal(t_assignments, Assignment::Priority::High);
    assignRolesInternal(t_assignments, Assignment::Priority::Medium);
    assignRolesInternal(t_assignments, Assignment::Priority::Low);
}

void Ai::assignRolesInternal(std::vector<Assignment> *const t_assignments, const Assignment::Priority t_priority)
{
    using namespace lemon;
    DIGRAPH_TYPEDEFS(SmartDigraph);

    SmartDigraph graph;

    // Build the example graph
    IntNodeMap        supply(graph);
    std::vector<Node> source;
    std::vector<Node> dest;

    std::set<int> assigned;
    for (const auto &assignment : *t_assignments)
    {
        if (assignment.new_assignee != -1)
            assigned.insert(assignment.new_assignee);
    }

    source.reserve(Common::Config::Common::kMaxRobots);
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        Node u    = graph.addNode();
        supply[u] = 1;
        source.push_back(u);
    }

    dest.reserve(t_assignments->size());
    for (int i = 0; i < t_assignments->size(); i++)
    {
        Node u    = graph.addNode();
        supply[u] = -1;
        dest.push_back(u);
    }

    IntArcMap cost_map(graph);

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
            const Assignment &assignment = (*t_assignments)[dst_idx];

            // Skip assignments that are not of the correct priority
            if (assignment.priority != t_priority)
                continue;

            // Skip assignments that have already been assigned
            if (assignment.new_assignee != -1)
                continue;

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
    typedef NetworkSimplex<SmartDigraph> NS;
    NS                                   ns(graph);
    ns.supplyType(NS::LEQ);
    ns.costMap(cost_map).supplyMap(supply);
    NS::ProblemType result = ns.run();

    // Print results

    Common::logDebug("Priority {}, optimal flow found: {}", (int) t_priority, result == NS::OPTIMAL ? "yes" : "no");
    Common::logDebug("Total cost: {}", ns.totalCost());
    Common::logDebug("Used arcs:");
    for (ArcIt a(graph); a != INVALID; ++a)
    {
        if (ns.flow(a) > 0)
        {
            const int src_idx = graph.id(graph.source(a));
            const int dst_idx = graph.id(graph.target(a)) - source.size();

            const int   robot_idx  = src_idx;
            Assignment &assignment = (*t_assignments)[dst_idx];

            assignment.new_assignee    = robot_idx;
            assignment.assignment_cost = cost_map[a];

            Common::logDebug("Robot: {}, Assignment: {}, ID: {}, cost: {}", robot_idx, assignment.current_assignee,
                             graph.id(a), cost_map[a]);
        }
    }
}
} // namespace Tyr::Soccer
