#include "errt.h"

namespace Tyr::Soccer
{
Planner::Planner(const int t_max_nodes) : m_max_nodes(t_max_nodes), m_tree(t_max_nodes)
{
    m_waypoints.reserve(m_max_nodes);
}

void Planner::init(Common::Vec2 init, Common::Vec2 final, float step)
{
    init_state  = nearestFree(init);
    final_state = nearestFree(final);
    m_step_size = step;

    m_tree.reset();
    m_tree.AddNode(init_state, nullptr);

    m_started_in_obs = obs_map.isInObstacle(init);

    /*init_state = init;
    final_state = nearestFree ( nearestFree ( final ) );
    m_step_size = step;

    m_tree.reset ( );
    m_tree.AddNode ( init_state , nullptr );

    if ( obs_map.isInObstacle ( init_state ) )
    {
            init = nearestFree ( nearestFree ( init ) );
            m_tree.AddNode ( init , m_tree.GetNode ( 0 ) );
    }*/
}

void Planner::setFieldParams(float _w, float _h)
{
    field_width  = _w;
    field_height = _h;
}

Common::Vec2 Planner::randomState()
{
    // return Common::Vec2 ( ( rnd ( ) * 10000.0 ) - 5000.0 , ( rnd ( ) * 10000.0 ) - 5000.0 );
    return Common::Vec2(((m_random.get() - 0.5f) * 2.0f * (field_width + 250.0f)),
                        ((m_random.get() - 0.5f) * 2.0f * (field_height + 250.0f)));
}

Common::Vec2 Planner::nearestFree(Common::Vec2 state)
{
    return nearestFreeProb(state);
    int r = 678, l = -678, u = 478, d = -478;

    state.x = std::min(678.0f, std::max(52.0f, state.x));
    state.y = std::min(478.0f, std::max(52.0f, state.y));

    if (!obs_map.isInObstacle(state))
        return state;

    for (int i = state.x; i < 678; i++)
    {
        if (!obs_map.isInObstacle(Common::Vec2(i, state.y)))
        {
            r = i - state.x;
            break;
        }
    }

    for (int i = state.x; i > 52; i--)
    {
        if (!obs_map.isInObstacle(Common::Vec2(i, state.y)))
        {
            l = i - state.x;
            break;
        }
    }

    for (int i = state.y; i < 478; i++)
    {
        if (!obs_map.isInObstacle(Common::Vec2(state.x, i)))
        {
            u = i - state.y;
            break;
        }
    }

    for (int i = state.y; i > 52; i--)
    {
        if (!obs_map.isInObstacle(Common::Vec2(state.x, i)))
        {
            d = i - state.y;
            break;
        }
    }

    int x, y;

    if (abs(r) < abs(l))
        x = r;
    else
        x = l;

    if (abs(u) < abs(d))
        y = u;
    else
        y = d;

    // Common::Vec2 ans = Common::Vec2 ( std::min ( 729 , std::max ( 0 , state.x + sgn ( x ) * abs ( y ) ) ) , std::min
    // ( 529 , std::max ( 0 , state.y + sgn ( y ) * abs ( x ) ) ) );
    Common::Vec2 ans = Common::Vec2(state.x + Common::sign(x) * std::abs(y), state.y + Common::sign(y) * std::abs(x));

    // if ( obs_map.isInObstacle ( ans ) )
    //	return nearestFree ( ans );

    float coss, sinn;
    coss = (state.x - ans.x) / ans.distanceTo(state);
    sinn = (state.y - ans.y) / ans.distanceTo(state);

    Common::Vec2 current = ans;

    while ((current.distanceTo(state) > 2) && (!obs_map.isInObstacle(current)))
    {
        ans = current;

        current.x += coss * 2.0f;
        current.y += sinn * 2.0f;
    }

    return ans;
}

Common::Vec2 Planner::nearestFreeProb(Common::Vec2 state)
{
    const float acceptable_free_dis = 50;
    if (!obs_map.isInObstacle(state))
        return state;

    Common::Vec2 ans    = state;
    float        minDis = (field_width + field_height) * 10.0f;

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

Common::Vec2 Planner::chooseTarget(int *type)
{
    float r = m_random.get();

    if (type == nullptr)
        type = new int;

    if (r <= goal_target_prob)
    {
        *type = 0;
        return final_state;
    }
    else if ((r <= goal_target_prob + waypoint_target_prob) && (m_cached_waypoints > 0) &&
             (m_cached_waypoints > m_cache_start))
    {
        r     = m_random.get();
        *type = (int) (r * (m_cached_waypoints));
        if (*type >= m_cached_waypoints)
            *type >= m_cached_waypoints;
        return m_waypoints[*type];
    }

    *type = -1;
    return randomState();
}

Node *Planner::extend(Node *s, Common::Vec2 &target)
{
    Common::Vec2 new_state;
    float        dx, dy, dis;
    dis = s->state.distanceTo(target);

    if (dis < m_step_size)
        return nullptr;

    // dx = std::min ( m_step_size , dis ) * ( target.x - s->state.x ) / dis;
    // dy = std::min ( m_step_size , dis ) * ( target.y - s->state.y ) / dis;

    dx = m_step_size * (target.x - s->state.x) / dis;
    dy = m_step_size * (target.y - s->state.y) / dis;

    new_state.x = s->state.x + dx;
    new_state.y = s->state.y + dy;

    // Boundary check
    // if ( ( new_state.x >= 605 ) || ( new_state.x < 0 ) || ( new_state.y >= 405 ) || ( new_state.y < 0 ) )
    //         return nullptr;

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

    if (isReached())
        m_cached_waypoints = m_waypoints.size();
    else
        m_cached_waypoints = 0;
    m_cache_start = 0;
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
    if (final_state.distanceTo(m_tree.NearestNeighbour(final_state)->state) <= acceptable_dis)
        return true;
    return false;
}

Common::Vec2 Planner::plan()
{
    // return final_state;
    if (!obs_map.collisionDetect(init_state, final_state))
    {
        m_tree.AddNode(final_state, m_tree.NearestNeighbour(init_state));
    }
    else
    {
        Node        *new_s;
        int          type = 0;
        Common::Vec2 r;
        for (int i = 0; ((i < m_max_nodes) && (!isReached())); i++)
        {
            r     = chooseTarget(&type);
            new_s = extend(m_tree.NearestNeighbour(r), r);
            if (new_s)
            {
                /*if ( !obs_map.collisionDetect ( new_s->state , final_state ) )
                {
                    for (; ( !isReached ( ) ) && ( new_s ) && (i < m_max_nodes ) ; i ++ )
                                new_s = extend ( new_s , final_state );
                        //m_tree.AddNode ( final_state , m_tree.NearestNeighbour ( r ) );
                }*/
            }
            // while ( !extend ( m_tree.NearestNeighbour ( r ) , r ) )
            //	r = chooseTarget ( );
        }
        if ((isReached()) && (!obs_map.isInObstacle(final_state)) &&
            (final_state.distanceTo(m_tree.NearestNeighbour(final_state)->state) > 1))
        {
            m_tree.AddNode(final_state, m_tree.NearestNeighbour(final_state));
        }
    }

    setWayPoints();

    // if ( ( ! obs_map.isInObstacle ( init_state ) ) && ( getWayPointNum() > 2 ) )
    optimize_tree();

    Common::Vec2 ans;
    if (m_waypoints.size() >= 2)
    {
        if (m_started_in_obs)
        {
            ans = m_waypoints[m_waypoints.size() - 1];
        }
        else
        {
            ans = m_waypoints[m_waypoints.size() - 2];
        }
    }
    else
    {
        ans = m_waypoints[0];
    }

    return ans;
}

void Planner::optimize_tree()
{
    for (int i = 0; i < getWayPointNum(); i++)
    {
        if (obs_map.collisionDetect(m_waypoints[i], m_waypoints[getWayPointNum() - 1]) == false)
        {
            std::swap(m_waypoints[i + 1], m_waypoints[getWayPointNum() - 1]);
            m_waypoints.resize(i + 2);
            break;
        }
    }
}
} // namespace Tyr::Soccer
