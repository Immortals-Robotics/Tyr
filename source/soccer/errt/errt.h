#pragma once

#include "../obstacle/obstacle.h"
#include "tree.h"

namespace Tyr::Soccer
{
class Planner
{
    float        step_size;
    Common::Vec2 init_state, final_state;
    Common::Vec2 waypoint[MAX_NODES];
    unsigned int waypoints, cached_waypoints, cache_start;

    void reverse_waypoints(void);

    float field_width;
    float field_height;

    bool started_in_obs;

    Common::Random random;

public:
    float goal_target_prob;
    float waypoint_target_prob;
    float acceptable_dis;

    Planner(void);
    ~Planner(void);
    Tree         tree;
    void         set_field_params(float _w, float _h);
    void         init(Common::Vec2 init, Common::Vec2 final, float step);
    Common::Vec2 random_state(void);
    Common::Vec2 nearest_free(Common::Vec2 state);
    Common::Vec2 nearest_free_prob(Common::Vec2 state);

    Common::Vec2 choose_target(int *type = NULL);
    Node        *extend(Node *s, Common::Vec2 &target);
    void         SetWayPoints(void);
    Common::Vec2 GetWayPoint(unsigned int i);
    unsigned int GetWayPointNum(void);
    bool         IsReached(void);

    void optimize_tree(void);

    Common::Vec2 Plan(void);
};
} // namespace Tyr::Soccer