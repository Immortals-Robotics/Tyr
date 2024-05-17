#pragma once

namespace Tyr::Soccer
{
inline Common::Vec2 Ai::ownGoal() const
{
    return Common::Vec2(m_side * Common::field().width, 0);
}

inline Common::Vec2 Ai::ownGoalPostTop() const
{
    return ownGoal() + Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
}

inline Common::Vec2 Ai::ownGoalPostBottom() const
{
    return ownGoal() - Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
}

inline Common::LineSegment Ai::ownGoalLine() const
{
    return {ownGoalPostBottom(), ownGoalPostTop()};
}

inline Common::Vec2 Ai::oppGoal() const
{
    return Common::Vec2(-m_side * Common::field().width, 0);
}

inline Common::Vec2 Ai::oppGoalPostTop() const
{
    return oppGoal() + Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
}

inline Common::Vec2 Ai::oppGoalPostBottom() const
{
    return oppGoal() - Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
}

inline Common::LineSegment Ai::oppGoalLine() const
{
    return {oppGoalPostBottom(), oppGoalPostTop()};
}

inline bool Ai::isOut(Common::Vec2 t_point, const float t_margin) const
{
    return std::fabs(t_point.x) > Common::field().width + t_margin ||
           std::fabs(t_point.y) > Common::field().height + t_margin;
}
} // namespace Tyr::Soccer
