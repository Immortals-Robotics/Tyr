#pragma once

namespace Tyr::Soccer
{
class Robot;
class State
{
public:
    static const Common::WorldState &world()
    {
        return *m_world;
    }

    static const Common::Referee::State &ref()
    {
        return *m_ref;
    }

    static int side()
    {
        return *m_side;
    }

    static const Robot *robots()
    {
        return m_robots;
    }

    static Common::Vec2 ownGoal()
    {
        return Common::Vec2(side() * Common::field().width, 0);
    }

    static Common::Vec2 ownGoalPostTop()
    {
        return ownGoal() + Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    static Common::Vec2 ownGoalPostBottom()
    {
        return ownGoal() - Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    static Common::LineSegment ownGoalLine()
    {
        return {ownGoalPostBottom(), ownGoalPostTop()};
    }

    static Common::Vec2 oppGoal()
    {
        return Common::Vec2(-side() * Common::field().width, 0);
    }

    static Common::Vec2 oppGoalPostTop()
    {
        return oppGoal() + Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    static Common::Vec2 oppGoalPostBottom()
    {
        return oppGoal() - Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    static Common::LineSegment oppGoalLine()
    {
        return {oppGoalPostBottom(), oppGoalPostTop()};
    }

    static Common::Rect ownPenaltyArea()
    {
        const float penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

        const Common::Vec2 start{side() * Common::field().width, -penalty_area_half_width};

        const float w = -side() * Common::field().penalty_area_depth;
        const float h = Common::field().penalty_area_width;

        return {start, w, h};
    }

    static Common::Rect oppPenaltyArea()
    {
        const float penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

        const Common::Vec2 start{-side() * Common::field().width, -penalty_area_half_width};

        const float w = side() * Common::field().penalty_area_depth;
        const float h = Common::field().penalty_area_width;

        return {start, w, h};
    }

    static bool isOut(const Common::Vec2 t_point, const float t_margin)
    {
        return std::fabs(t_point.x) > Common::field().width + t_margin ||
               std::fabs(t_point.y) > Common::field().height + t_margin;
    }

private:
    friend class Ai;

    static inline const Common::WorldState     *m_world;
    static inline const Common::Referee::State *m_ref;

    static inline const int   *m_side;
    static inline const Robot *m_robots;
};

} // namespace Tyr::Soccer
