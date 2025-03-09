#include "errt.h"

namespace Tyr::Soccer
{
Planner::Planner(const int t_max_nodes, const float t_step)
    : m_max_nodes(t_max_nodes)
    , m_step_size(t_step)
    , m_tree(t_max_nodes)
{
    m_waypoints.reserve(m_max_nodes);
    m_cached_waypoints.reserve(m_max_nodes);
}

Common::Vec2 Planner::nearestFree(const Common::Vec2 state)
{
    const float acceptable_free_dis = 50.0f;

    if (!m_map->isInObstacle(state))
        return state;

    Common::Vec2 ans    = state;
    float        minDis = std::numeric_limits<float>::max();

    for (int i = 0; i < 1000; i++)
    {
        Common::Vec2 newRndPoint = randomState();
        const float  tmp_d       = state.distanceSquaredTo(newRndPoint);
        if (!m_map->isInObstacle(newRndPoint) && tmp_d < minDis)
        {
            ans    = newRndPoint;
            minDis = tmp_d;
            if (minDis < acceptable_free_dis)
                break;
        }
    }

    return ans;
}

Node *Planner::extend(Node *s, Common::Vec2 &target)
{
    if (s->state.distanceTo(target) < m_step_size)
        return nullptr;

    Common::Vec2 step_vec = (target - s->state).normalized() * m_step_size;

    Common::Vec2 new_state = s->state + step_vec;

    // collision check
    if (m_map->isInObstacle(new_state))
        return nullptr;

    if (m_map->collisionDetect(new_state, s->state))
        return nullptr;

    return m_tree.addNode(new_state, s);
}

void Planner::setWayPoints()
{
    m_waypoints.clear();

    Node *n = m_tree.nearestNeighbour(final_state);
    m_waypoints.push_back(n->state);

    while (n->parent)
    {
        n = n->parent;
        m_waypoints.push_back(n->state);
    }

    m_cached_waypoints.clear();
    if (isReached())
        m_cached_waypoints = m_waypoints;
}

Common::Vec2 Planner::plan(const Common::Vec2 init, const Common::Vec2 final)
{
    init_state  = nearestFree(init);
    final_state = nearestFree(final);

    m_tree.reset();
    m_tree.addNode(init_state, nullptr);

    m_started_in_obs = m_map->isInObstacle(init);

    if (m_cached_waypoints.empty())
    {
        Common::logWarning("cached waypoints are empty");
    }

    // return final_state;
    if (!m_map->collisionDetect(init_state, final_state))
    {
        // TODO: slice the path so that the cache contains valid waypoints
        m_tree.addNode(final_state, m_tree.nearestNeighbour(final_state));
    }
    else
    {
        for (int i = 0; ((i < m_max_nodes) && (!isReached())); i++)
        {
            Common::Vec2 r = chooseTarget();
            extend(m_tree.nearestNeighbour(r), r);
        }

        if ((isReached()) && (!m_map->isInObstacle(final_state)) &&
            (final_state.distanceTo(m_tree.nearestNeighbour(final_state)->state) > 1))
        {
            m_tree.addNode(final_state, m_tree.nearestNeighbour(final_state));
        }
    }

    setWayPoints();
    optimizeTree();

    if (m_started_in_obs || m_waypoints.size() <= 1)
    {
        return m_waypoints.back();
    }
    else
    {
        return m_waypoints[m_waypoints.size() - 2];
    }
}

void Planner::optimizeTree()
{
    for (size_t i = 0; i < m_waypoints.size() - 1; i++)
    {
        if (m_map->collisionDetect(m_waypoints[i], m_waypoints.back()) == false)
        {
            std::swap(m_waypoints[i + 1], m_waypoints.back());
            m_waypoints.resize(i + 2);
            break;
        }
    }
}

void Planner::draw(const Common::Color t_color) const
{
    for (size_t i = 0; i < m_waypoints.size() - 1; i++)
    {
        Common::debug().draw(Common::LineSegment{m_waypoints[i], m_waypoints[i + 1]}, t_color, 5.0f);
    }
}
} // namespace Tyr::Soccer
