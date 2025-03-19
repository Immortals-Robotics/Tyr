#pragma once

#include "../trajectory/trajectory.h"
#include "circle.h"
#include "rect.h"

namespace Tyr::Soccer
{
class ObstacleMap
{
private:
    std::vector<CircleObstacle> m_circles;
    std::vector<RectObstacle>   m_rects;

public:
    ObstacleMap() = default;

    void add(Common::Circle t_circle)
    {
        m_circles.emplace_back(t_circle);

        Common::debug().draw(t_circle, Common::Color::maroon().transparent(), false);
    }

    void add(Common::Rect t_rect)
    {
        m_rects.emplace_back(t_rect);

        Common::debug().draw(t_rect, Common::Color::maroon().transparent(), false);
    }

    bool  inside(Common::Vec2 t_point) const;
    float distance(Common::Vec2 t_point) const;

    // TODO: change the param to line segment
    bool collisionDetect(Common::Vec2 p1, Common::Vec2 p2) const;

    template <TrajectoryConcept2D TrajectoryA, TrajectoryConcept2D TrajectoryB>
    std::pair<bool, float> hasCollision(const TrajectoryA &a, const TrajectoryB &b,
        const float r, float look_ahead = 3.0f, float step_t = 0.1f) const
    {
        const float t_start = std::max(a.getStartTime(), b.getStartTime());
        const float t_end_raw = std::min(a.getEndTime(), b.getEndTime());
        const float t_end = std::min(t_end_raw, t_start + look_ahead);

        for (float t = t_start; t < t_end; t += step_t)
        {
            const Common::Vec2 pos_a = a.getPosition(t);
            const Common::Vec2 pos_b = b.getPosition(t);

            if (pos_a.distanceTo(pos_b) <= r)
            {
                return {true, t};
            }
        }

        return {false, std::numeric_limits<float>::max()};
    }

    template <TrajectoryConcept2D Trajectory>
    std::pair<bool, float> hasCollision(const Trajectory& trajectory, const float look_ahead = 3.0f, const float step_t = 0.1f) const
    {
        const float t_end = std::min(trajectory.getEndTime(), trajectory.getStartTime() + look_ahead);

        for (float t = trajectory.getStartTime(); t < t_end; t += step_t)
        {
            const Common::Vec2 pos = trajectory.getPosition(t);

            if (inside(pos))
            {
                return {true, t};
            }
        }

        return {false, std::numeric_limits<float>::max()};
    }

    // if the trajectory starts with collision, will it ever get out?
    template <TrajectoryConcept2D Trajectory>
    std::pair<bool, float> reachFree(const Trajectory &trajectory, float look_ahead = 3.0f, float step_t = 0.1f) const
    {
        const float t_end = std::min(trajectory.getEndTime(), trajectory.getStartTime() + look_ahead);

        for (float t = trajectory.getStartTime(); t < t_end; t += step_t)
        {
            const Common::Vec2 pos = trajectory.getPosition(t);

            if (!inside(pos))
            {
                return {true, t};
            }
        }

        return {false, std::numeric_limits<float>::max()};
    }

    void resetMap()
    {
        m_circles.clear();
        m_rects.clear();
    }
};

} // namespace Tyr::Soccer
