#pragma once

namespace Tyr::Soccer
{
class Field
{
public:
    static Common::Vec2 ownGoal()
    {
        return Common::Vec2(State::side() * Common::field().width, 0);
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
        return Common::Vec2(-State::side() * Common::field().width, 0);
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

        const Common::Vec2 start{State::side() * Common::field().width, -penalty_area_half_width};

        const float w = -State::side() * Common::field().penalty_area_depth;
        const float h = Common::field().penalty_area_width;

        return {start, w, h};
    }

    static Common::Rect oppPenaltyArea()
    {
        const float penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

        const Common::Vec2 start{-State::side() * Common::field().width, -penalty_area_half_width};

        const float w = State::side() * Common::field().penalty_area_depth;
        const float h = Common::field().penalty_area_width;

        return {start, w, h};
    }

    static Common::Rect ourHalf()
    {
        const float height = Common::field().height + Common::field().boundary_width;

        const Common::Vec2 corner{
            0.0f,
            -height};

        return Common::Rect{corner, State::side() * Common::field().width, 2.0f * height};
    }

    static Common::Rect theirHalf()
    {
        const float height = Common::field().height + Common::field().boundary_width;

        const Common::Vec2 corner{
            0.0f,
            -height};

        return Common::Rect{corner, - State::side() * Common::field().width, 2.0f * height};
    }

    static bool isOut(const Common::Vec2 t_point, const float t_margin = 0.0f)
    {
        return std::fabs(t_point.x) > Common::field().width + t_margin ||
               std::fabs(t_point.y) > Common::field().height + t_margin;
    }
};
} // namespace Tyr::Soccer
