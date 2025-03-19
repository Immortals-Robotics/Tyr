#include "../ai.h"

namespace Tyr::Soccer
{
static int hys_dive = 0, hys_select = 0;

void Ai::defHi(int t_def1_num, int t_def2_num, Common::Vec2 *t_defend_target)
{
    auto predicted_ball = predictBall(Common::config().soccer.def_prediction_time).position;
    if (t_defend_target != nullptr)
    {
        predicted_ball = *t_defend_target;
    }

    if (ballIsGoaling() && m_ref_state.running())
    {
        defShirje(t_def1_num, t_def2_num);
        hys_dive = 10;
    }
    else if (hys_dive > 0 && m_ref_state.running())
    {
        defShirje(t_def1_num, t_def2_num);
        hys_dive--;
    }
    else
    {

        float area_extension_size = predicted_ball.distanceTo(ownGoal()) * 0.7 - Common::field().penalty_area_depth;
        area_extension_size       = std::min(1100.f, area_extension_size);
        area_extension_size       = std::max(Common::field().robot_radius, area_extension_size);
        const float penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

        const Common::Vec2 start{ownGoal().x, -(penalty_area_half_width + area_extension_size)};
        const float        w = -m_side * (area_extension_size + Common::field().penalty_area_depth);
        const float        h = Common::field().penalty_area_width + 2 * area_extension_size;

        const Common::Rect virtual_defense_area{start, w, h};

        const auto start_ang_effect = Common::config().soccer.def_tight_start_angle;
        const auto ball_angle =
            (ownGoal() - predicted_ball).normalized().toAngle() - Common::Angle::fromDeg(m_side == -1 ? 180.f : 0.f);
        const auto ball_angle_corrected =
            std::min(std::max(0.f, std::fabs(ball_angle.deg()) - (90.f - start_ang_effect)), start_ang_effect) *
            (ball_angle.deg() >= 0.f ? 1.f : -1.f);

        Common::Line line_1 =
            Common::Line::fromTwoPoints(predicted_ball, ownGoal() + Common::Vec2(0, Common::field().goal_width / 2));
        Common::Line line_2 =
            Common::Line::fromTwoPoints(predicted_ball, ownGoal() - Common::Vec2(0, Common::field().goal_width / 2));

        if (std::fabs(ball_angle_corrected) > 0.f)
        {
            const Common::Vec2 limit_ball =
                ownGoal() -
                Common::Angle::fromDeg((90.f - start_ang_effect) * (ball_angle.deg() >= 0.f ? 1.f : -1.f)).toUnitVec() *
                    (predicted_ball.distanceTo(ownGoal())) * (m_side);

            if (ball_angle_corrected * m_side >= 0.f)
            {
                line_1 = Common::Line::fromTwoPoints(limit_ball,
                                                     ownGoal() + Common::Vec2(0, Common::field().goal_width / 2));
            }
            else
            {
                line_2 = Common::Line::fromTwoPoints(limit_ball,
                                                     ownGoal() + Common::Vec2(0, Common::field().goal_width / 2));
            }
            Common::debug().draw(limit_ball, Common::Color::red());
        }

        const Common::Line ball_goal_line = Common::Line::fromTwoPoints(predicted_ball, ownGoal());

        const auto   intersects_1 = virtual_defense_area.intersection(line_1);
        const auto   intersects_2 = virtual_defense_area.intersection(line_2);
        Common::Vec2 final_pos_1{(predicted_ball - ownGoal()).normalized() *
                                     (penalty_area_half_width + Common::field().robot_radius + 20.f) +
                                 ownGoal()};
        Common::Vec2 final_pos_2{(predicted_ball - ownGoal()).normalized() *
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
            if (intersect.distanceTo(predicted_ball) < final_pos_1.distanceTo(predicted_ball))
            {
                final_pos_1 = intersect;
            }
        }
        for (const auto &intersect : intersects_2)
        {
            if (intersect.distanceTo(predicted_ball) < final_pos_2.distanceTo(predicted_ball))
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

        Common::debug().draw(final_pos_1, Common::Color::red());
        Common::debug().draw(final_pos_2, Common::Color::yellow());

        Common::debug().draw(virtual_defense_area, Common::Color::blue(), false);

        m_own_robot[t_def1_num].face(final_pos_1 + (final_pos_1 - ownGoal()).normalized() * 10000.);
        m_own_robot[t_def1_num].navigate(final_pos_1, VelocityProfile::mamooli());
        m_own_robot[t_def2_num].face(final_pos_2 + (final_pos_2 - ownGoal()).normalized() * 10000.);
        m_own_robot[t_def2_num].navigate(final_pos_2, VelocityProfile::mamooli());
    }
}

void Ai::defShirje(const int t_def_1, const int t_def_2)
{
    Common::Line ball_line =
        Common::Line::fromPointAndAngle(m_world_state.ball.position, m_world_state.ball.velocity.toAngle());
    Common::Vec2 one_touch_1  = ball_line.closestPoint(m_own_robot[t_def_1].state().position);
    Common::Vec2 one_touch_2  = ball_line.closestPoint(m_own_robot[t_def_2].state().position);
    int          dive_def_num = 2;
    if (one_touch_1.distanceTo(m_own_robot[t_def_1].state().position) <
            one_touch_2.distanceTo(m_own_robot[t_def_2].state().position))
    {
        dive_def_num = 1;
        hys_select   = 10;
    }

    hys_select--;
    hys_select = std::max(hys_select, 0);

    if (dive_def_num == 1)
    {
        m_own_robot[t_def_1].face(m_world_state.ball.position);
        m_own_robot[t_def_1].chip(150);
        const Common::Line tangent            = ball_line.tangentLine(one_touch_1);
        const Common::Vec2 nearest_to_tangent = tangent.closestPoint(m_own_robot[t_def_2].state().position);
        one_touch_2 =
            one_touch_1 + (nearest_to_tangent - one_touch_1).normalized() * (Common::field().robot_radius * 2.f);
    }
    else
    {
        m_own_robot[t_def_2].face(m_world_state.ball.position);
        m_own_robot[t_def_2].chip(150);
        const Common::Line tangent            = ball_line.tangentLine(one_touch_2);
        const Common::Vec2 nearest_to_tangent = tangent.closestPoint(m_own_robot[t_def_1].state().position);
        one_touch_1 =
            one_touch_2 + (nearest_to_tangent - one_touch_2).normalized() * (Common::field().robot_radius * 2.f);
    }

    Common::debug().draw(Common::Circle{one_touch_1, 50}, Common::Color::yellow());
    Common::debug().draw(Common::Circle{one_touch_2, 50}, Common::Color::yellow());

    m_own_robot[t_def_1].navigate(one_touch_1, VelocityProfile::kharaki());
    m_own_robot[t_def_2].navigate(one_touch_2, VelocityProfile::kharaki());
}
} // namespace Tyr::Soccer
