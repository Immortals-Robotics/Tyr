#include "renderer.h"

namespace Tyr::Gui
{
void Renderer::draw(const Common::FieldState &t_field)
{
    ClearBackground(GREEN);

    Common::Vec2 fieldWallStartPoint =
        Common::Vec2(-t_field.width - t_field.boundary_width, -t_field.height - t_field.boundary_width);
    Common::Vec2 fieldWallEndPoint =
        Common::Vec2(t_field.width + t_field.boundary_width, t_field.height + t_field.boundary_width);
    Common::Vec2 fieldStartPoint = Common::Vec2(-t_field.width, t_field.height * 2.0f / -2);
    Common::Vec2 fieldEndPoint   = Common::Vec2(t_field.width, t_field.height * 2.0f / 2);
    Common::Vec2 fieldCenter     = Common::Vec2(0, 0);

    Common::Vec2 ourGoalStartPoint = Common::Vec2(-t_field.width - t_field.goal_depth, t_field.goal_width / 2);
    Common::Vec2 ourGoalEndPoint   = Common::Vec2(-t_field.width, t_field.goal_width / -2);
    Common::Vec2 oppGoalStartPoint = -ourGoalStartPoint;
    Common::Vec2 oppGoalEndPoint   = -ourGoalEndPoint;

    Common::Vec2 ourPenaltyStartPoint = Common::Vec2(-t_field.width, t_field.penalty_area_width / 2);
    Common::Vec2 ourPenaltyEndPoint =
        Common::Vec2(-t_field.width + t_field.penalty_area_depth, t_field.penalty_area_width / -2);

    Common::Vec2 oppenaltyStartPoint = -ourPenaltyStartPoint;
    Common::Vec2 oppPenaltyEndPoint  = -ourPenaltyEndPoint;

    Common::Vec2 centerLineStartPoint = Common::Vec2(0, -t_field.height);
    Common::Vec2 centerLineEndPoint   = -centerLineStartPoint;

    float centerCircleRad = t_field.center_circle_radius;

    draw(Common::Rect{fieldWallStartPoint, fieldWallEndPoint}, Common::Color::brown(), false, 70.0f);

    static constexpr float kLineWidth = 20.0f;

    draw(Common::Rect{fieldEndPoint, fieldStartPoint}, Common::Color::white(), false, kLineWidth);
    draw(Common::Rect{ourGoalEndPoint, ourGoalStartPoint}, Common::Color::white(), false, kLineWidth);
    draw(Common::Rect{oppGoalStartPoint, oppGoalEndPoint}, Common::Color::white(), false, kLineWidth);
    draw(Common::Rect{ourPenaltyStartPoint, ourPenaltyEndPoint}, Common::Color::white(), false, kLineWidth);
    draw(Common::Rect{oppenaltyStartPoint, oppPenaltyEndPoint}, Common::Color::white(), false, kLineWidth);
    draw(Common::LineSegment{centerLineStartPoint, centerLineEndPoint}, Common::Color::white(), kLineWidth);
    draw(Common::Circle{fieldCenter, centerCircleRad}, Common::Color::white(), false, kLineWidth);

    if (ImGui::IsMouseClicked(0))
    {
        m_clicked_pos = mousePosition();
    }
    else if (ImGui::IsMouseClicked(1))
    {
        m_clicked_pos.reset();
    }

    if (m_clicked_pos.has_value())
    {
        draw(Common::Circle{m_clicked_pos.value(), 50}, Common::Color::red(), true, 170);
    }
}
} // namespace Tyr::Gui
