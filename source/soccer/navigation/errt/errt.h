#pragma once

#include "../obstacle/map.h"
#include "tree.h"

namespace Tyr::Soccer
{
class Planner
{
public:
     Planner(int t_max_nodes = 2000, float t_step = 90.f);
    ~Planner() = default;

    Common::Vec2 plan(Common::Vec2 init, Common::Vec2 final);

    void draw(Common::Color t_color = Common::Color::black().transparent()) const;

    void setObstacleMap(const ObstacleMap* const t_map)
    {
        m_map = t_map;
    }

private:
    const ObstacleMap* m_map = nullptr;

    Common::Vec2 init_state;
    Common::Vec2 final_state;

    std::vector<Common::Vec2> m_waypoints;
    std::vector<Common::Vec2> m_cached_waypoints;

    float m_step_size;

    int m_max_nodes;

    bool m_started_in_obs;

    // TODO: move to setting
    float goal_target_prob     = 0.1f;
    float waypoint_target_prob = 0.8f;
    float acceptable_dis       = 90.0f;

    Common::Random m_random;

    Tree m_tree;

    Common::Vec2 randomState()
    {
        return Common::Vec2((m_random.get(-1.0f, 1.0f) * (Common::field().width + Common::field().boundary_width)),
                            (m_random.get(-1.0f, 1.0f) * (Common::field().height + Common::field().boundary_width)));
    }

    Common::Vec2 nearestFree(Common::Vec2 state);

    Common::Vec2 chooseTarget();
    Node        *extend(Node *s, Common::Vec2 &target);
    void         setWayPoints();

    Common::Vec2 getWayPoint(const unsigned int i) const
    {
        return m_waypoints[i];
    }

    unsigned int getWayPointNum() const
    {
        return m_waypoints.size();
    }

    bool isReached()
    {
        return final_state.distanceTo(m_tree.nearestNeighbour(final_state)->state) <= acceptable_dis;
    }

    void optimizeTree();
};

inline Common::Vec2 Planner::chooseTarget()
{
    const float r = m_random.get(0.0f, 1.0f);

    if (r <= goal_target_prob)
    {
        return final_state;
    }
    else if ((r <= goal_target_prob + waypoint_target_prob) && (m_cached_waypoints.size() > 0))
    {
        const int idx = m_random.get(0, static_cast<int>(m_cached_waypoints.size()) - 1);
        return m_cached_waypoints[idx];
    }
    else
    {
        return randomState();
    }
}

} // namespace Tyr::Soccer
