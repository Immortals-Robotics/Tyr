#include "def.h"

#include "../helpers/ball_is_goaling.h"
#include "../helpers/ball_prediction.h"

namespace Tyr::Soccer
{
const Tactic::Id DefTactic::kId = &DefTactic::kId;

void DefTactic::execute(Robot &t_robot)
{
    if (ballIsGoaling())
        m_hys_dive = 10;
    else
        m_hys_dive = std::max(m_hys_dive - 1, 0);

    if (State::ref().running() && m_hys_dive > 0)
    {
        if (m_num == 1)
            defShirje_1(t_robot);
        else if (m_num == 2)
            defShirje_2(t_robot);
    }
    else
    {
        t_robot.face(State::world().ball.position);

        Common::Vec2 static_pos{};
        if (m_num == 1)
            static_pos = defStatic_1();
        else if (m_num == 2)
            static_pos = defStatic_2();
        t_robot.navigate(static_pos);
    }
}

Common::Vec2 DefTactic::defStatic_1()
{
    const Common::Vec2 predicted_ball = predictBall(Common::config().soccer.def_prediction_time).position;

    float area_extension_size = predicted_ball.distanceTo(Field::ownGoal()) * 0.7 - Common::field().penalty_area_depth;
    area_extension_size       = std::min(1100.f, area_extension_size);
    area_extension_size       = std::max(Common::field().robot_radius, area_extension_size);
    const float penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

    const Common::Vec2 start{Field::ownGoal().x, -(penalty_area_half_width + area_extension_size)};
    const float        w = -State::side() * (area_extension_size + Common::field().penalty_area_depth);
    const float        h = Common::field().penalty_area_width + 2 * area_extension_size;

    const Common::Rect virtual_defense_area{start, w, h};

    const auto start_ang_effect = Common::config().soccer.def_tight_start_angle;
    const auto ball_angle       = (Field::ownGoal() - predicted_ball).normalized().toAngle() -
                            Common::Angle::fromDeg(State::side() == -1 ? 180.f : 0.f);
    const auto ball_angle_corrected =
        std::min(std::max(0.f, std::fabs(ball_angle.deg()) - (90.f - start_ang_effect)), start_ang_effect) *
        (ball_angle.deg() >= 0.f ? 1.f : -1.f);

    Common::Line line_1 =
        Common::Line::fromTwoPoints(predicted_ball, Field::ownGoal() + Common::Vec2(0, Common::field().goal_width / 2));

    if (std::fabs(ball_angle_corrected) > 0.f)
    {
        const Common::Vec2 limit_ball =
            Field::ownGoal() -
            Common::Angle::fromDeg((90.f - start_ang_effect) * (ball_angle.deg() >= 0.f ? 1.f : -1.f)).toUnitVec() *
                (predicted_ball.distanceTo(Field::ownGoal())) * (State::side());

        if (ball_angle_corrected * State::side() >= 0.f)
        {
            line_1 = Common::Line::fromTwoPoints(limit_ball,
                                                 Field::ownGoal() + Common::Vec2(0, Common::field().goal_width / 2));
        }
        Common::debug().draw(limit_ball, Common::Color::red());
    }

    const Common::Line ball_goal_line = Common::Line::fromTwoPoints(predicted_ball, Field::ownGoal());

    const auto   intersects_1 = virtual_defense_area.intersection(line_1);
    Common::Vec2 final_pos_1{(predicted_ball - Field::ownGoal()).normalized() *
                                 (penalty_area_half_width + Common::field().robot_radius + 20.f) +
                             Field::ownGoal()};

    if (intersects_1.size())
    {
        final_pos_1 = intersects_1.front();
    }
    for (const auto &intersect : intersects_1)
    {
        if (intersect.distanceTo(predicted_ball) < final_pos_1.distanceTo(predicted_ball))
        {
            final_pos_1 = intersect;
        }
    }

    const auto tangent_line_1 = line_1.tangentLine(final_pos_1);

    const auto mid_point_1 = tangent_line_1.intersect(ball_goal_line);

    if (mid_point_1.has_value())
    {
        final_pos_1 = final_pos_1 + (mid_point_1.value() - final_pos_1).normalized() * Common::field().robot_radius;
    }

    Common::debug().draw(final_pos_1, Common::Color::red());

    Common::debug().draw(virtual_defense_area, Common::Color::blue(), false);

    return final_pos_1;
}

Common::Vec2 DefTactic::defStatic_2()
{
    const Common::Vec2 predicted_ball = predictBall(Common::config().soccer.def_prediction_time).position;

    float area_extension_size = predicted_ball.distanceTo(Field::ownGoal()) * 0.7 - Common::field().penalty_area_depth;
    area_extension_size       = std::min(1100.f, area_extension_size);
    area_extension_size       = std::max(Common::field().robot_radius, area_extension_size);
    const float penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

    const Common::Vec2 start{Field::ownGoal().x, -(penalty_area_half_width + area_extension_size)};
    const float        w = -State::side() * (area_extension_size + Common::field().penalty_area_depth);
    const float        h = Common::field().penalty_area_width + 2 * area_extension_size;

    const Common::Rect virtual_defense_area{start, w, h};

    const auto start_ang_effect = Common::config().soccer.def_tight_start_angle;
    const auto ball_angle       = (Field::ownGoal() - predicted_ball).normalized().toAngle() -
                            Common::Angle::fromDeg(State::side() == -1 ? 180.f : 0.f);
    const auto ball_angle_corrected =
        std::min(std::max(0.f, std::fabs(ball_angle.deg()) - (90.f - start_ang_effect)), start_ang_effect) *
        (ball_angle.deg() >= 0.f ? 1.f : -1.f);

    Common::Line line_2 =
        Common::Line::fromTwoPoints(predicted_ball, Field::ownGoal() - Common::Vec2(0, Common::field().goal_width / 2));

    if (std::fabs(ball_angle_corrected) > 0.f)
    {
        const Common::Vec2 limit_ball =
            Field::ownGoal() -
            Common::Angle::fromDeg((90.f - start_ang_effect) * (ball_angle.deg() >= 0.f ? 1.f : -1.f)).toUnitVec() *
                (predicted_ball.distanceTo(Field::ownGoal())) * (State::side());

        if (ball_angle_corrected * State::side() < 0.f)
        {
            line_2 = Common::Line::fromTwoPoints(limit_ball,
                                                 Field::ownGoal() + Common::Vec2(0, Common::field().goal_width / 2));
        }
        Common::debug().draw(limit_ball, Common::Color::red());
    }

    const Common::Line ball_goal_line = Common::Line::fromTwoPoints(predicted_ball, Field::ownGoal());

    const auto   intersects_2 = virtual_defense_area.intersection(line_2);
    Common::Vec2 final_pos_2{(predicted_ball - Field::ownGoal()).normalized() *
                                 (penalty_area_half_width + Common::field().robot_radius + 20.f) +
                             Field::ownGoal()};
    if (intersects_2.size())
    {
        final_pos_2 = intersects_2.front();
    }
    for (const auto &intersect : intersects_2)
    {
        if (intersect.distanceTo(predicted_ball) < final_pos_2.distanceTo(predicted_ball))
        {
            final_pos_2 = intersect;
        }
    }

    const auto tangent_line_2 = line_2.tangentLine(final_pos_2);

    const auto mid_point_2 = tangent_line_2.intersect(ball_goal_line);

    if (mid_point_2.has_value())
    {
        final_pos_2 = final_pos_2 + (mid_point_2.value() - final_pos_2).normalized() * Common::field().robot_radius;
    }

    Common::debug().draw(final_pos_2, Common::Color::yellow());

    Common::debug().draw(virtual_defense_area, Common::Color::blue(), false);

    return final_pos_2;
}

void DefTactic::defShirje_1(Robot &t_robot_1)
{
    const Common::Vec2 def_1_pos = defStatic_1();
    const Common::Vec2 def_2_pos = defStatic_2();

    Common::Line ball_line =
        Common::Line::fromPointAndAngle(State::world().ball.position, State::world().ball.velocity.toAngle());
    Common::Vec2 one_touch_1  = ball_line.closestPoint(def_1_pos);
    Common::Vec2 one_touch_2  = ball_line.closestPoint(def_2_pos);
    int          dive_def_num = 2;
    if (one_touch_1.distanceTo(def_1_pos) < one_touch_2.distanceTo(def_2_pos))
    {
        dive_def_num = 1;
        s_hys_select = 10;
    }

    s_hys_select--;
    s_hys_select = std::max(s_hys_select, 0);

    if (dive_def_num == 1)
    {
        t_robot_1.face(State::world().ball.position);
        t_robot_1.chip(150);
        const Common::Line tangent            = ball_line.tangentLine(one_touch_1);
        const Common::Vec2 nearest_to_tangent = tangent.closestPoint(def_2_pos);
        one_touch_2 =
            one_touch_1 + (nearest_to_tangent - one_touch_1).normalized() * (Common::field().robot_radius * 2.f);
    }
    else
    {
        const Common::Line tangent            = ball_line.tangentLine(one_touch_2);
        const Common::Vec2 nearest_to_tangent = tangent.closestPoint(t_robot_1.state().position);
        one_touch_1 =
            one_touch_2 + (nearest_to_tangent - one_touch_2).normalized() * (Common::field().robot_radius * 2.f);
    }

    Common::debug().draw(Common::Circle{one_touch_1, 50}, Common::Color::yellow());
    Common::debug().draw(Common::Circle{one_touch_2, 50}, Common::Color::yellow());

    t_robot_1.navigate(one_touch_1, VelocityProfile::kharaki());
}

void DefTactic::defShirje_2(Robot &t_robot_2)
{
    const Common::Vec2 def_1_pos = defStatic_1();
    const Common::Vec2 def_2_pos = defStatic_2();

    Common::Line ball_line =
        Common::Line::fromPointAndAngle(State::world().ball.position, State::world().ball.velocity.toAngle());
    Common::Vec2 one_touch_1  = ball_line.closestPoint(def_1_pos);
    Common::Vec2 one_touch_2  = ball_line.closestPoint(def_2_pos);
    int          dive_def_num = 2;
    if (one_touch_1.distanceTo(def_1_pos) < one_touch_2.distanceTo(def_2_pos))
    {
        dive_def_num = 1;
        s_hys_select = 10;
    }

    s_hys_select--;
    s_hys_select = std::max(s_hys_select, 0);

    if (dive_def_num == 1)
    {
        const Common::Line tangent            = ball_line.tangentLine(one_touch_1);
        const Common::Vec2 nearest_to_tangent = tangent.closestPoint(t_robot_2.state().position);
        one_touch_2 =
            one_touch_1 + (nearest_to_tangent - one_touch_1).normalized() * (Common::field().robot_radius * 2.f);
    }
    else
    {
        t_robot_2.face(State::world().ball.position);
        t_robot_2.chip(150);
        const Common::Line tangent            = ball_line.tangentLine(one_touch_2);
        const Common::Vec2 nearest_to_tangent = tangent.closestPoint(def_1_pos);
        one_touch_1 =
            one_touch_2 + (nearest_to_tangent - one_touch_2).normalized() * (Common::field().robot_radius * 2.f);
    }

    Common::debug().draw(Common::Circle{one_touch_1, 50}, Common::Color::yellow());
    Common::debug().draw(Common::Circle{one_touch_2, 50}, Common::Color::yellow());

    t_robot_2.navigate(one_touch_2, VelocityProfile::kharaki());
}
} // namespace Tyr::Soccer
