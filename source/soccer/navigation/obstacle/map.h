#pragma once

#include "../trajectory/trajectory.h"
#include "obstacle.h"

namespace Tyr::Soccer
{
class ObstacleMap
{
private:
    std::vector<CircleObstacle> m_circles;
    std::vector<RectObstacle>   m_rects;

    Physicality m_physicality = Physicality::All;

    float m_base_margin = 0.0f;

public:
    ObstacleMap() = default;

    void add(const Common::Circle t_circle, const Physicality t_physicality)
    {
        m_circles.emplace_back(t_circle, t_physicality);

        Common::debug().draw(t_circle, Common::Color::maroon().transparent(), false);
    }

    void add(const Common::Rect t_rect, const Physicality t_physicality)
    {
        m_rects.emplace_back(t_rect, t_physicality);

        Common::debug().draw(t_rect, Common::Color::maroon().transparent(), false);
    }

    bool  inside(Common::Vec2 t_point, float t_margin = 0.0f, Physicality t_physicality = Physicality::All) const;
    float distance(Common::Vec2 t_point, Physicality t_physicality = Physicality::All) const;

    // TODO: change the param to line segment
    bool collisionDetect(Common::Vec2 p1, Common::Vec2 p2, Physicality t_physicality = Physicality::All) const;

    template <TrajectoryConcept2D Trajectory>
    std::pair<bool, float> hasCollision(const Trajectory& trajectory, const float look_ahead = 3.0f, const float step_t = 0.1f, const Physicality t_physicality = Physicality::All) const
    {
        const float t_end = std::min(trajectory.getEndTime(), trajectory.getStartTime() + look_ahead);

        for (float t = trajectory.getStartTime(); t < t_end; t += step_t)
        {
            const Common::Vec2 pos = trajectory.getPosition(t);

            if (inside(pos, 0.0f, t_physicality))
            {
                return {true, t};
            }
        }

        return {false, std::numeric_limits<float>::max()};
    }

    // if the trajectory starts with collision, will it ever get out?
    template <TrajectoryConcept2D Trajectory>
    std::pair<bool, float> reachFree(const Trajectory &trajectory, float look_ahead = 3.0f, float step_t = 0.1f, const Physicality t_physicality = Physicality::All) const
    {
        const float t_end = std::min(trajectory.getEndTime(), trajectory.getStartTime() + look_ahead);

        for (float t = trajectory.getStartTime(); t < t_end; t += step_t)
        {
            const Common::Vec2 pos = trajectory.getPosition(t);

            if (!inside(pos, 0.0f, t_physicality))
            {
                return {true, t};
            }
        }

        return {false, std::numeric_limits<float>::max()};
    }

    Physicality physicality() const
    {
        return m_physicality;
    }

    void setPhysicality(const Physicality t_physicality)
    {
        m_physicality = t_physicality;
    }

    float baseMargin() const
    {
        return m_base_margin;
    }

    void setBaseMargin(const float t_margin)
    {
        m_base_margin = t_margin;
    }

    void reset()
    {
        m_circles.clear();
        m_rects.clear();

        m_physicality = Physicality::All;

        m_base_margin = 0.0f;
    }
};

} // namespace Tyr::Soccer
