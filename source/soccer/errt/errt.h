#pragma once

#include "../obstacle/obstacle.h"
#include "tree.h"

namespace Tyr::Soccer
{
class Planner
{
private:
    Common::Vec2 init_state;
    Common::Vec2 final_state;

    std::vector<Common::Vec2> m_waypoints;

    int m_cached_waypoints = 0;

    float m_step_size;

    int m_max_nodes;

    float field_width  = 0.0f;
    float field_height = 0.0f;

    bool m_started_in_obs;

    // TODO: move to setting
    float goal_target_prob     = 0.1f;
    float waypoint_target_prob = 0.5f;
    float acceptable_dis       = 90.0f;

    Common::Random m_random;

    Tree m_tree;

    Common::Vec2 randomState();
    Common::Vec2 nearestFree(Common::Vec2 state);

    Common::Vec2 chooseTarget();
    Node        *extend(Node *s, Common::Vec2 &target);
    void         setWayPoints();
    Common::Vec2 getWayPoint(unsigned int i);
    unsigned int getWayPointNum();
    bool         isReached();

    void optimize_tree();

public:
    Planner(int t_max_nodes = 1000);
    ~Planner() = default;

    void setFieldParams(float _w, float _h);
    void init(Common::Vec2 init, Common::Vec2 final, float step);

    Common::Vec2 plan();
};
} // namespace Tyr::Soccer
