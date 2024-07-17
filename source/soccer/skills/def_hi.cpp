#include "../ai.h"

namespace Tyr::Soccer
{

void Ai::defHi(int t_robot_num, int t_right_def_num, int t_left_def_num, Common::Vec2 *t_defend_target)
{
    (void) t_left_def_num;

    auto  predicted_ball      = m_world_state.ball.position;
    float area_extension_size = predicted_ball.distanceTo(ownGoal()) * 0.6 - Common::field().penalty_area_depth;
    area_extension_size       = std::min(1800.f, area_extension_size);
    area_extension_size       = std::max(Common::field().robot_radius, area_extension_size);
    const float penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

    const Common::Vec2 start{ownGoal().x, -(penalty_area_half_width + area_extension_size)};
    const float        w = -m_side * (area_extension_size + Common::field().penalty_area_depth);
    const float        h = Common::field().penalty_area_width + 2 * area_extension_size;

    const Common::Rect virtual_defense_area{start, w, h};

    const Common::Line line_1 = Common::Line::fromTwoPoints(
        m_world_state.ball.position, ownGoal() + Common::Vec2(0, Common::field().goal_width / 2));
    const Common::Line line_2 = Common::Line::fromTwoPoints(
        m_world_state.ball.position, ownGoal() - Common::Vec2(0, Common::field().goal_width / 2));

    const Common::Line ball_goal_line = Common::Line::fromTwoPoints(predicted_ball, ownGoal());

    const auto   intersects_1 = virtual_defense_area.intersection(line_1);
    const auto   intersects_2 = virtual_defense_area.intersection(line_2);
    Common::Vec2 final_pos_1{(m_world_state.ball.position - ownGoal()).normalized() *
                                 (penalty_area_half_width + Common::field().robot_radius + 20.f) +
                             ownGoal()};
    Common::Vec2 final_pos_2{(m_world_state.ball.position - ownGoal()).normalized() *
                                 (penalty_area_half_width + Common::field().robot_radius + 20.f) +
                             ownGoal()};
    if (intersects_1.size())
    {
        final_pos_1 = intersects_1.front();
    }
    if (intersects_2.size())
    {
        final_pos_2 = intersects_2.front();
    }
    for (const auto &intersect : intersects_1)
    {
        if (intersect.distanceTo(m_world_state.ball.position) < final_pos_1.distanceTo(m_world_state.ball.position))
        {
            final_pos_1 = intersect;
        }
    }
    for (const auto &intersect : intersects_2)
    {
        if (intersect.distanceTo(m_world_state.ball.position) < final_pos_2.distanceTo(m_world_state.ball.position))
        {
            final_pos_2 = intersect;
        }
    }

    const auto tangent_line_1 = line_1.tangentLine(final_pos_1);
    const auto tangent_line_2 = line_2.tangentLine(final_pos_2);

    const auto mid_point_1 = tangent_line_1.intersect(ball_goal_line);
    const auto mid_point_2 = tangent_line_2.intersect(ball_goal_line);

    if (mid_point_1.has_value())
    {
        final_pos_1 = final_pos_1 + (mid_point_1.value() - final_pos_1).normalized() * Common::field().robot_radius;
    }
    if (mid_point_2.has_value())
    {
        final_pos_2 = final_pos_2 + (mid_point_2.value() - final_pos_2).normalized() * Common::field().robot_radius;
    }
    //
    Common::debug().draw(final_pos_1, Common::Color::red());
    Common::debug().draw(final_pos_2, Common::Color::yellow());

    Common::debug().draw(virtual_defense_area, Common::Color::blue(), false);

    m_own_robot[t_robot_num].face(final_pos_1 + (final_pos_1 - ownGoal()).normalized() * 10000.);
    navigate(t_robot_num, final_pos_1, VelocityProfile::mamooli());
    m_own_robot[t_right_def_num].face(final_pos_2 + (final_pos_2 - ownGoal()).normalized() * 10000.);
    navigate(t_right_def_num, final_pos_2, VelocityProfile::mamooli());

    return;
    if (!t_defend_target)
        t_defend_target = &(m_world_state.ball.position);

    Common::Angle alpha = ownGoal().angleWith(m_world_state.ball.position) + Common::Angle::fromDeg(90 + m_side * 90);
    alpha.setDeg(std::clamp(alpha.deg(), -90.0f, 90.0f));
    float alphaSgn = Common::sign(alpha.deg());

    if (t_robot_num != -1)
    {
        if (std::fabs(alpha.deg()) < 43.0)
        {
            Common::Line GOAL_LINE = Common::Line::fromSegment(ownGoalLine());

            Common::Vec2 TARGET_BALL_IN_GOAL = GOAL_LINE.closestPoint(m_world_state.ball.position);

            if (TARGET_BALL_IN_GOAL.y > 300)
            {
                TARGET_BALL_IN_GOAL.y = 300.0;
            }
            if (TARGET_BALL_IN_GOAL.y < -300)
            {
                TARGET_BALL_IN_GOAL.y = -300.0;
            }

            Common::Line ball_line  = Common::Line::fromTwoPoints(m_world_state.ball.position, TARGET_BALL_IN_GOAL);
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(m_side * (Common::field().width - Common::field().penalty_area_depth - 100), 0),
                Common::Angle::fromDeg(90.0));
            Common::Vec2 target = ball_line.intersect(Front_line).value_or(Common::Vec2());

            runningDef(t_robot_num, target, t_defend_target);
        }
        else
        {
            Common::Line ball_line =
                Common::Line::fromPointAndAngle(ownGoal(), Common::Angle::fromDeg(alphaSgn * 43.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(m_side * (Common::field().width - Common::field().penalty_area_depth - 100), 0),
                Common::Angle::fromDeg(90.0));
            Common::Vec2 fans = ball_line.intersect(Front_line).value_or(Common::Vec2());

            m_own_robot[t_robot_num].target.angle = Common::Angle::fromDeg(alphaSgn * 43.0 + 90 + m_side * 90);
            navigate(t_robot_num, fans, VelocityProfile::mamooli());
        }
    }

    const bool right_def_marking = std::any_of(m_mark_map.begin(), m_mark_map.end(), [t_right_def_num](const auto &pair)
                                               { return *pair.first == t_right_def_num && pair.second != -1; });
    if (t_right_def_num != -1 && !right_def_marking)
    {
        // t_right_def_num
        if (alpha.deg() < -85.0)
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(ownGoal(), Common::Angle::fromDeg(-85.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 fans = ball_line.intersect(Front_line).value_or(Common::Vec2());

            m_own_robot[t_right_def_num].target.angle = Common::Angle::fromDeg(-85.0 + 90 + m_side * 90);
            navigate(t_right_def_num, fans, VelocityProfile::mamooli());
        }
        else if (alpha.deg() < -48.0)
        {
            Common::Line ball_line  = Common::Line::fromTwoPoints(m_world_state.ball.position, ownGoal());
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 target = ball_line.intersect(Front_line).value_or(Common::Vec2());

            runningDef(t_right_def_num, target, t_defend_target);
        }
        else
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(ownGoal(), Common::Angle::fromDeg(-48.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 fans = ball_line.intersect(Front_line).value_or(Common::Vec2());

            m_own_robot[t_right_def_num].target.angle = Common::Angle::fromDeg(-48.0 + 90 + m_side * 90);
            navigate(t_right_def_num, fans, VelocityProfile::mamooli());
        }
    }

    const bool left_def_marking = std::any_of(m_mark_map.begin(), m_mark_map.end(), [t_left_def_num](const auto &pair)
                                              { return *pair.first == t_left_def_num && pair.second != -1; });
    if (t_left_def_num != -1 && !left_def_marking)
    {
        // t_left_def_num
        if (alpha.deg() > 85.0)
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(ownGoal(), Common::Angle::fromDeg(85.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 fans = ball_line.intersect(Front_line).value_or(Common::Vec2());

            m_own_robot[t_left_def_num].target.angle = Common::Angle::fromDeg(85.0 + 90 + m_side * 90);
            navigate(t_left_def_num, fans, VelocityProfile::mamooli());
        }
        else if (alpha.deg() > 48.0)
        {
            Common::Line ball_line  = Common::Line::fromTwoPoints(m_world_state.ball.position, ownGoal());
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 target = ball_line.intersect(Front_line).value_or(Common::Vec2());

            runningDef(t_left_def_num, target, t_defend_target);
        }
        else
        {
            Common::Line ball_line  = Common::Line::fromPointAndAngle(ownGoal(), Common::Angle::fromDeg(48.0));
            Common::Line Front_line = Common::Line::fromPointAndAngle(
                Common::Vec2(0, -m_side * (Common::field().penalty_area_depth + 100)), Common::Angle::fromDeg(0.0));
            Common::Vec2 fans = ball_line.intersect(Front_line).value_or(Common::Vec2());

            m_own_robot[t_left_def_num].target.angle = Common::Angle::fromDeg(48.0 + 90 + m_side * 90);
            navigate(t_left_def_num, fans, VelocityProfile::mamooli());
        }
    }
}
} // namespace Tyr::Soccer
