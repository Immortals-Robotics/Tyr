#include "errt.h"

namespace Tyr::Soccer
{
Planner::Planner(const int t_max_nodes) : m_max_nodes(t_max_nodes), m_tree(t_max_nodes)
{
    m_waypoints.reserve(m_max_nodes);
    m_cached_waypoints.reserve(m_max_nodes);
}

void Planner::init(Common::Vec2 init, Common::Vec2 final, float step)
{
    init_state  = nearestFree(init);
    final_state = nearestFree(final);
    m_step_size = step;

    m_tree.reset();
    m_tree.AddNode(init_state, nullptr);

    m_started_in_obs = obs_map.isInObstacle(init);
}

void Planner::setFieldParams(float _w, float _h)
{
    field_width  = _w;
    field_height = _h;
}

Common::Vec2 Planner::randomState()
{
    return Common::Vec2((m_random.get(-1.0f, 1.0f) * (field_width + 250.0f)),
                        (m_random.get(-1.0f, 1.0f) * (field_height + 250.0f)));
}

Common::Vec2 Planner::nearestFree(Common::Vec2 state)
{
    const float acceptable_free_dis = 50.0f;

    if (!obs_map.isInObstacle(state))
        return state;

    Common::Vec2 ans    = state;
    float        minDis = std::numeric_limits<float>::max();

    for (int i = 0; i < 1000; i++)
    {
        Common::Vec2 newRndPoint = randomState();
        if ((!obs_map.isInObstacle(newRndPoint)) && state.distanceTo(newRndPoint) < minDis)
        {
            ans    = newRndPoint;
            minDis = state.distanceTo(newRndPoint);
            if (minDis < acceptable_free_dis)
                break;
        }
    }

    return ans;
}

Common::Vec2 Planner::chooseTarget()
{
    float r = m_random.get();

    if (r <= goal_target_prob)
    {
        return final_state;
    }
    else if ((r <= goal_target_prob + waypoint_target_prob) && (m_cached_waypoints.size() > 0))
    {
        r       = m_random.get();
        int idx = (int) (r * (m_cached_waypoints.size()));
        idx     = std::clamp(idx, 0, (int) m_cached_waypoints.size() - 1);
        return m_cached_waypoints[idx];
    }

    return randomState();
}

Node *Planner::extend(Node *s, Common::Vec2 &target)
{
    if (s->state.distanceTo(target) < m_step_size)
        return nullptr;

    Common::Vec2 step_vec = (target - s->state).normalized() * m_step_size;

    Common::Vec2 new_state = s->state + step_vec;

    // collision check
    if (obs_map.isInObstacle(new_state))
        return nullptr;

    if (obs_map.collisionDetect(new_state, s->state))
        return nullptr;

    return m_tree.AddNode(new_state, s);
}

void Planner::setWayPoints()
{
    m_waypoints.clear();

    Node *n = m_tree.NearestNeighbour(final_state);
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

Common::Vec2 Planner::getWayPoint(unsigned int i)
{
    return m_waypoints[i];
}

unsigned int Planner::getWayPointNum()
{
    return m_waypoints.size();
}

bool Planner::isReached()
{
    return final_state.distanceTo(m_tree.NearestNeighbour(final_state)->state) <= acceptable_dis;
}

Common::Vec2 Planner::plan()
{
    // return final_state;
    if (!obs_map.collisionDetect(init_state, final_state))
    {
        m_tree.AddNode(final_state, m_tree.NearestNeighbour(final_state));
    }
    else
    {
        for (int i = 0; ((i < m_max_nodes) && (!isReached())); i++)
        {
            Common::Vec2 r     = chooseTarget();
            Node        *new_s = extend(m_tree.NearestNeighbour(r), r);
        }

        if ((isReached()) && (!obs_map.isInObstacle(final_state)) &&
            (final_state.distanceTo(m_tree.NearestNeighbour(final_state)->state) > 1))
        {
            m_tree.AddNode(final_state, m_tree.NearestNeighbour(final_state));
        }
    }

    setWayPoints();
    optimize_tree();

    if (m_started_in_obs || m_waypoints.size() <= 1)
    {
        return m_waypoints.back();
    }
    else
    {
        return m_waypoints[m_waypoints.size() - 2];
    }
}

void Planner::optimize_tree()
{
    for (int i = 0; i < m_waypoints.size() - 1; i++)
    {
        if (obs_map.collisionDetect(m_waypoints[i], m_waypoints.back()) == false)
        {
            std::swap(m_waypoints[i + 1], m_waypoints.back());
            m_waypoints.resize(i + 2);
            break;
        }
    }
}
} // namespace Tyr::Soccer
