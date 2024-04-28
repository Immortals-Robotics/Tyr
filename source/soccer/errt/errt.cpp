#include "errt.h"

#define map2worldX(a) ((a - 364) * 10)
#define map2worldY(a) ((a - 264) * 10)

#define world2mapX(a) std::min(729, std::max(0, ((a / 10) + 364)))
#define world2mapY(a) std::min(529, std::max(0, ((a / 10) + 264)))

namespace Tyr::Soccer
{
Planner::Planner(void)
{
    waypoints        = 0;
    cached_waypoints = 0;
    cache_start      = 0;

    goal_target_prob     = 0.1;
    waypoint_target_prob = 0.5;
    acceptable_dis       = 90.0;

    field_width  = 0;
    field_height = 0;
}

Planner::~Planner()
{}

void Planner::init(Common::Vec2 init, Common::Vec2 final, float step)
{

    init_state  = nearest_free(init);
    final_state = nearest_free(final);
    step_size   = step;

    tree.reset();
    tree.AddNode(init_state, NULL);

    started_in_obs = IsInObstacle(init);

    /*init_state = init;
    final_state = nearest_free ( nearest_free ( final ) );
    step_size = step;

    tree.reset ( );
    tree.AddNode ( init_state , NULL );

    if ( IsInObstacle ( init_state ) )
    {
            init = nearest_free ( nearest_free ( init ) );
            tree.AddNode ( init , tree.GetNode ( 0 ) );
    }*/
}

void Planner::set_field_params(float _w, float _h)
{
    field_width  = _w;
    field_height = _h;
}

Common::Vec2 Planner::random_state(void)
{
    // return Common::Vec2 ( ( rnd ( ) * 10000.0 ) - 5000.0 , ( rnd ( ) * 10000.0 ) - 5000.0 );
    return Common::Vec2(((random.get() - 0.5f) * 2.0f * (field_width + 250.0f)),
                        ((random.get() - 0.5f) * 2.0f * (field_height + 250.0f)));
}

Common::Vec2 Planner::nearest_free(Common::Vec2 state)
{
    return nearest_free_prob(state);
    int r = 678, l = -678, u = 478, d = -478;

    state.x = std::min(678.0f, std::max(52.0f, state.x));
    state.y = std::min(478.0f, std::max(52.0f, state.y));

    if (!IsInObstacle(state))
        return state;

    for (int i = state.x; i < 678; i++)
    {
        if (!IsInObstacle(Common::Vec2(i, state.y)))
        {
            r = i - state.x;
            break;
        }
    }

    for (int i = state.x; i > 52; i--)
    {
        if (!IsInObstacle(Common::Vec2(i, state.y)))
        {
            l = i - state.x;
            break;
        }
    }

    for (int i = state.y; i < 478; i++)
    {
        if (!IsInObstacle(Common::Vec2(state.x, i)))
        {
            u = i - state.y;
            break;
        }
    }

    for (int i = state.y; i > 52; i--)
    {
        if (!IsInObstacle(Common::Vec2(state.x, i)))
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

    // if ( IsInObstacle ( ans ) )
    //	return nearest_free ( ans );

    float coss, sinn;
    coss = (state.x - ans.x) / ans.distanceTo(state);
    sinn = (state.y - ans.y) / ans.distanceTo(state);

    Common::Vec2 current = ans;

    while ((current.distanceTo(state) > 2) && (!IsInObstacle(current)))
    {
        ans = current;

        current.x += coss * 2.0f;
        current.y += sinn * 2.0f;
    }

    return ans;
}

Common::Vec2 Planner::nearest_free_prob(Common::Vec2 state)
{
    const float acceptable_free_dis = 50;
    if (!IsInObstacle(state))
        return state;

    Common::Vec2 ans    = state;
    float        minDis = (field_width + field_height) * 10.0f;

    for (int i = 0; i < 1000; i++)
    {
        Common::Vec2 newRndPoint = random_state();
        if ((!IsInObstacle(newRndPoint)) && state.distanceTo(newRndPoint) < minDis)
        {
            ans    = newRndPoint;
            minDis = state.distanceTo(newRndPoint);
            if (minDis < acceptable_free_dis)
                break;
        }
    }

    return ans;
}

Common::Vec2 Planner::choose_target(int *type)
{
    float r = random.get();

    if (type == NULL)
        type = new int;

    if (r <= goal_target_prob)
    {
        *type = 0;
        return final_state;
    }
    else if ((r <= goal_target_prob + waypoint_target_prob) && (cached_waypoints > 0) &&
             (cached_waypoints > cache_start))
    {
        r     = random.get();
        *type = (int) (r * (cached_waypoints));
        if (*type >= cached_waypoints)
            *type >= cached_waypoints;
        return waypoint[*type];
    }

    *type = -1;
    return random_state();
}

Node *Planner::extend(Node *s, Common::Vec2 &target)
{
    Common::Vec2 new_state;
    float        dx, dy, dis;
    dis = s->state.distanceTo(target);

    if (dis < step_size)
        return NULL;

    // dx = std::min ( step_size , dis ) * ( target.x - s->state.x ) / dis;
    // dy = std::min ( step_size , dis ) * ( target.y - s->state.y ) / dis;

    dx = step_size * (target.x - s->state.x) / dis;
    dy = step_size * (target.y - s->state.y) / dis;

    new_state.x = s->state.x + dx;
    new_state.y = s->state.y + dy;

    // Boundary check
    // if ( ( new_state.x >= 605 ) || ( new_state.x < 0 ) || ( new_state.y >= 405 ) || ( new_state.y < 0 ) )
    //         return NULL;

    // collision check
    if (IsInObstacle(new_state))
        return NULL;

    if (collisionDetect(new_state, s->state))
        return NULL;

    return tree.AddNode(new_state, s);
}

void Planner::SetWayPoints(void)
{
    waypoints   = 1;
    Node *n     = tree.NearestNeighbour(final_state);
    waypoint[0] = n->state;

    while (n->parent)
    {
        n                     = n->parent;
        waypoint[waypoints++] = n->state;
    }

    if (IsReached())
        cached_waypoints = waypoints;
    else
        cached_waypoints = 0;
    cache_start = 0;
}

void Planner::reverse_waypoints(void)
{
    Common::Vec2 tmp;
    for (int i = 0; i < waypoints / 2; i++)
    {
        tmp                         = waypoint[i];
        waypoint[i]                 = waypoint[waypoints - i - 1];
        waypoint[waypoints - i - 1] = tmp;
    }
}

Common::Vec2 Planner::GetWayPoint(unsigned int i)
{
    if (i < waypoints)
        return waypoint[i];
    return Common::Vec2();
}

unsigned int Planner::GetWayPointNum(void)
{
    return waypoints;
}

bool Planner::IsReached(void)
{
    if (final_state.distanceTo(tree.NearestNeighbour(final_state)->state) <= acceptable_dis)
        return true;
    return false;
}

Common::Vec2 Planner::Plan(void)
{
    // return final_state;
    if (!collisionDetect(init_state, final_state))
    {
        tree.AddNode(final_state, tree.NearestNeighbour(init_state));
    }
    else
    {
        Node        *new_s;
        int          type = 0;
        Common::Vec2 r;
        for (int i = 0; ((i < MAX_NODES) && (!IsReached())); i++)
        {
            r     = choose_target(&type);
            new_s = extend(tree.NearestNeighbour(r), r);
            if (new_s)
            {
                /*if ( !collisionDetect ( new_s->state , final_state ) )
                {
                    for (; ( !IsReached ( ) ) && ( new_s ) && (i < MAX_NODES ) ; i ++ )
                                new_s = extend ( new_s , final_state );
                        //tree.AddNode ( final_state , tree.NearestNeighbour ( r ) );
                }*/
            }
            // while ( !extend ( tree.NearestNeighbour ( r ) , r ) )
            //	r = choose_target ( );
        }
        if ((IsReached()) && (!IsInObstacle(final_state)) &&
            (final_state.distanceTo(tree.NearestNeighbour(final_state)->state) > 1))
        {
            tree.AddNode(final_state, tree.NearestNeighbour(final_state));
        }
    }

    SetWayPoints();

    // if ( ( ! IsInObstacle ( init_state ) ) && ( GetWayPointNum() > 2 ) )
    optimize_tree();

    Common::Vec2 ans;
    if (waypoints > 1)
    {
        if (started_in_obs)
        {
            ans.x = waypoint[waypoints - 1].x;
            ans.y = waypoint[waypoints - 1].y;
        }
        else
        {
            ans.x = waypoint[waypoints - 2].x;
            ans.y = waypoint[waypoints - 2].y;
        }
    }
    else
    {
        ans.x = waypoint[0].x;
        ans.y = waypoint[0].y;
    }

    return ans;
}

void Planner::optimize_tree(void)
{
    for (int i = 0; i < GetWayPointNum(); i++)
    {
        if (collisionDetect(waypoint[i], waypoint[GetWayPointNum() - 1]) == false)
        {
            Common::Vec2 tmp               = waypoint[i + 1];
            waypoint[i + 1]                = waypoint[GetWayPointNum() - 1];
            waypoint[GetWayPointNum() - 1] = tmp;
            waypoints                      = i + 2;
            break;
        }
    }
}
} // namespace Tyr::Soccer
