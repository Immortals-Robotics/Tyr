#pragma once

#include "../obstacle/obstacle.h"
#include "tree.h"

namespace Tyr::Soccer
{
class Planner
{
private:
    int m_max_nodes;

    Common::Vec2              init_state, final_state;
    std::vector<Common::Vec2> m_waypoints;

    int waypoints        = 0;
    int cached_waypoints = 0;
    int cache_start      = 0;

    float step_size;

    float field_width  = 0.0f;
    float field_height = 0.0f;

    bool started_in_obs;

    Common::Random random;

    // TODO: move to setting
    float goal_target_prob     = 0.1f;
    float waypoint_target_prob = 0.5f;
    float acceptable_dis       = 90.0f;

    void reverse_waypoints();

public:
    Planner(const int t_max_nodes = 1000) : m_max_nodes(t_max_nodes), tree(t_max_nodes)
    {
        m_waypoints.resize(m_max_nodes);
    }
    ~Planner() = default;

    Tree         tree;
    void         set_field_params(float _w, float _h);
    void         init(Common::Vec2 init, Common::Vec2 final, float step);
    Common::Vec2 random_state();
    Common::Vec2 nearest_free(Common::Vec2 state);
    Common::Vec2 nearest_free_prob(Common::Vec2 state);

    Common::Vec2 choose_target(int *type = NULL);
    Node        *extend(Node *s, Common::Vec2 &target);
    void         SetWayPoints();
    Common::Vec2 GetWayPoint(unsigned int i);
    unsigned int GetWayPointNum();
    bool         IsReached();

    void optimize_tree();

    Common::Vec2 Plan();
};
} // namespace Tyr::Soccer
