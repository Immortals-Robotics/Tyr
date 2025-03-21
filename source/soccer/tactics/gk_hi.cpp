#include "../ai.h"

#include "../helpers/ball_prediction.h"
#include "../helpers/ball_is_goaling.h"

#include "attacker.h"

namespace Tyr::Soccer
{
static int my_hys = 0;

void Ai::gkHi(Robot& t_robot)
{
    m_gk_intercepting = false;

    Common::logDebug("GKhi: {} _ {}", ballIsGoaling(),
                     m_world_state.ball.position.distanceTo(t_robot.state().position) /
                         m_world_state.ball.velocity.length());
    if (ballIsGoaling())
    {
        Common::debug().draw(Common::Circle{t_robot.state().position, 100}, Common::Color::red(),
                             false);
    }
    else
    {
        Common::debug().draw(Common::Circle{t_robot.state().position, 100}, Common::Color::yellow(),
                             false);
    }

    if ((ballIsGoaling()) &&
        (m_world_state.ball.position.distanceTo(t_robot.state().position) /
             m_world_state.ball.velocity.length() <
         3) &&
        m_ref_state.running())
    {
        gkShirje(t_robot);
        my_hys = 10;
    }
    else if ((my_hys > 0) && m_ref_state.running())
    {
        gkShirje(t_robot);
        my_hys--;
    }

    else
    {
        my_hys                            = 0;
        const Common::Vec2 predicted_ball = predictBall(Common::config().soccer.def_prediction_time).position;
        ObstacleMap obs_map;

        // our penalty area
        static constexpr float area_extension_size     = 200.0f;
        static constexpr float area_notch              = 200.0f;
        const float            penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

        const Common::Vec2 start{Field::ownGoal().x, -(penalty_area_half_width + area_extension_size)};

        const float        w = -m_side * (area_extension_size + Common::field().penalty_area_depth);
        const float        h = Common::field().penalty_area_width + 2 * area_extension_size;
        const Common::Line goal_line =
            Common::Line::fromTwoPoints(Field::ownGoal() - Common::Vec2(0, 1000), Field::ownGoal() + Common::Vec2(0, 1000));
        float      ball_position_effect = goal_line.distanceTo(predicted_ball);
        const auto start_ang_effect     = Common::config().soccer.gk_tight_start_angle;
        const auto ball_angle =
            std::min(std::max(0.f, std::fabs(((Field::ownGoal() - predicted_ball).normalized().toAngle() -
                                              Common::Angle::fromDeg(m_side == -1 ? 180.f : 0.f))
                                                 .deg()) -
                                       (90.f - start_ang_effect)),
                     start_ang_effect);
        const auto gk_max_dist = penalty_area_half_width - area_notch;
        const auto ball_ang_effect =
            gk_max_dist - ((gk_max_dist - (Common::field().goal_width / 2.f - Common::field().robot_radius)) / start_ang_effect) * ball_angle;
        const float gk_target_w =
            -m_side * std::min((Common::field().penalty_area_depth - area_notch), ball_position_effect);
        const float        gk_target_h = Common::field().penalty_area_width - 2 * area_notch;
        const Common::Vec2 gk_target_area_start{Field::ownGoal().x, -(penalty_area_half_width - area_notch)};
        Common::logDebug("ang {} pos {} pt {}", ball_angle, ball_ang_effect, penalty_area_half_width);
        obs_map.add({start, w, h});

        if ((obs_map.inside(predicted_ball)) && (m_world_state.ball.velocity.length() < 1500) &&
            m_ref_state.running())
        {
            Common::logDebug("GK intercepting");

            m_gk_intercepting = true;

            AttackerTactic{predicted_ball.angleWith(Common::Vec2(m_side * (Common::field().width + 110), 0)), 0,
                     20, 0, 0}.execute(t_robot);
        }
        else
        {
            Common::Rect gk_target_rect{gk_target_area_start, gk_target_w, gk_target_h};
            Common::Line ball_goal_line = Common::Line::fromTwoPoints(predicted_ball, Field::ownGoal());
            auto         intersects     = gk_target_rect.intersection(ball_goal_line);
            Common::Vec2 gk_final_pos{Field::ownGoal()};
            for (const auto &intersect : intersects)
            {
                Common::debug().draw(intersect, Common::Color::blue());
                if (intersect.distanceTo(predicted_ball) <
                    gk_final_pos.distanceTo(predicted_ball))
                {
                    gk_final_pos = intersect;
                }
                Common::logDebug("x y : {}", intersect.x, intersect.y);
            }
            const float slope        = 0.0001538461538f;
            float       speed_effect = slope * m_world_state.ball.velocity.length();
            speed_effect             = std::min(speed_effect, 0.9f);
            speed_effect             = std::max(speed_effect, 0.f);
            gk_final_pos -= (gk_final_pos - Field::ownGoal()) * speed_effect;
            if (ball_angle > 0.f && (gk_final_pos - Field::ownGoal()).length() > ball_ang_effect) {
                gk_final_pos = (predicted_ball - Field::ownGoal()).normalized()*ball_ang_effect + Field::ownGoal();
            }
            Common::logDebug("final {} side {} un {}", gk_final_pos.x, m_side, Field::ownGoal().x - (m_side * (Common::field().robot_radius + 20.f)));

            if(std::fabs(gk_final_pos.x * m_side) >= std::fabs(Field::ownGoal().x - (m_side * (Common::field().robot_radius + 20.f)))) {
                gk_final_pos.x = Field::ownGoal().x - (m_side * (Common::field().robot_radius + 20.f));
            }
            Common::debug().draw(ball_goal_line, Common::Color::red());

            t_robot.face((gk_final_pos - Field::ownGoal()).normalized() * 10000. +
                                          Field::ownGoal()); // m_world_state.ball.position);
            t_robot.navigate(gk_final_pos, VelocityProfile::mamooli(), NavigationFlags::ForceNoOwnPenaltyArea);
        }
    }
}

void Ai::gkShirje(Robot& t_robot)
{
    Common::logDebug("GK Shirje");

    Common::Line ball_line =
        Common::Line::fromPointAndAngle(m_world_state.ball.position, m_world_state.ball.velocity.toAngle());
    Common::Vec2 ans = ball_line.closestPoint(t_robot.state().position);
    t_robot.face(m_world_state.ball.position);
    //    ans = ((ans - t_robot.state().position) * 2.0f) + t_robot.state().position;
    t_robot.navigate(ans, VelocityProfile::kharaki(), NavigationFlags::ForceNoOwnPenaltyArea);
    t_robot.chip(150);
}
} // namespace Tyr::Soccer
