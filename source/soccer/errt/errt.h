#pragma once

#include "../obstacle/map.h"
#include "tree.h"

namespace Tyr::Soccer
{
class Planner
{
public:
    Planner(int t_max_nodes = 1000);
    ~Planner() = default;

    void init(Common::Vec2 init, Common::Vec2 final, float step);

    Common::Vec2 plan();

    void draw(Common::Color t_color = Common::Color::white()) const;

private:
    Common::Vec2 init_state;
    Common::Vec2 final_state;

    std::vector<Common::Vec2> m_waypoints;
    std::vector<Common::Vec2> m_cached_waypoints;

    float m_step_size;

    int m_max_nodes;

    bool m_started_in_obs;

    // TODO: move to setting
    float goal_target_prob     = 0.1f;
    float waypoint_target_prob = 0.5f;
    float acceptable_dis       = 90.0f;

    Common::Random m_random;

    Tree m_tree;

    inline Common::Vec2 randomState()
    {
        return Common::Vec2((m_random.get(-1.0f, 1.0f) * (Common::field().width + 250.0f)),
                            (m_random.get(-1.0f, 1.0f) * (Common::field().height + 250.0f)));
    }

    Common::Vec2 nearestFree(Common::Vec2 state);

    Common::Vec2 chooseTarget();
    Node        *extend(Node *s, Common::Vec2 &target);
    void         setWayPoints();

    inline Common::Vec2 getWayPoint(unsigned int i)
    {
        return m_waypoints[i];
    }

    inline unsigned int getWayPointNum()
    {
        return m_waypoints.size();
    }

    inline bool isReached()
    {
        return final_state.distanceTo(m_tree.NearestNeighbour(final_state)->state) <= acceptable_dis;
    }

    void optimize_tree();
};

inline Common::Vec2 Planner::chooseTarget()
{
    float r = m_random.get(0.0f, 1.0f);

    if (r <= goal_target_prob)
    {
        return final_state;
    }
    else if ((r <= goal_target_prob + waypoint_target_prob) && (m_cached_waypoints.size() > 0))
    {
        const int idx = m_random.get(0, (int) m_cached_waypoints.size() - 1);
        return m_cached_waypoints[idx];
    }

    else
    {
        return randomState();
    }
}

} // namespace Tyr::Soccer
