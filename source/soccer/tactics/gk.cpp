#include "gk.h"

#include "../robot/robot.h"
#include "../helpers/ball_is_goaling.h"
#include "../helpers/ball_prediction.h"

#include "../skills/old_attacker.h"

namespace Tyr::Soccer
{
const Tactic::Id GkTactic::kId = &GkTactic::kId;

void GkTactic::execute(Robot &t_robot)
{
    m_intercepting = false;

    Common::logDebug("GKhi: {} _ {}", ballIsGoaling(),
                     State::world().ball.position.distanceTo(t_robot.state().position) /
                     State::world().ball.velocity.length());
    if (ballIsGoaling())
    {
        Common::debug().draw(Common::Circle{t_robot.state().position, 100}, Common::Color::red(), false);
    }
    else
    {
        Common::debug().draw(Common::Circle{t_robot.state().position, 100}, Common::Color::yellow(), false);
    }

    if ((ballIsGoaling()) &&
        (State::world().ball.position.distanceTo(t_robot.state().position) / State::world().ball.velocity.length() <
         3) &&
        State::ref().running())
    {
        shirje(t_robot);
        m_hys = 10;
    }
    else if ((m_hys > 0) && State::ref().running())
    {
        shirje(t_robot);
        m_hys--;
    }

    else
    {
        m_hys                             = 0;
        const Common::Vec2 predicted_ball = predictBall(Common::config().soccer.def_prediction_time).position;

        // our penalty area
        static constexpr float area_extension_size     = 200.0f;
        static constexpr float area_notch              = 200.0f;
        const float            penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

        const Common::Vec2 start{Field::ownGoal().x, -(penalty_area_half_width + area_extension_size)};

        const float        w         = -State::side() * (area_extension_size + Common::field().penalty_area_depth);
        const float        h         = Common::field().penalty_area_width + 2 * area_extension_size;
        const Common::Line goal_line = Common::Line::fromTwoPoints(Field::ownGoal() - Common::Vec2(0, 1000),
                                                                   Field::ownGoal() + Common::Vec2(0, 1000));
        float      ball_position_effect = goal_line.distanceTo(predicted_ball);
        const auto start_ang_effect     = Common::config().soccer.gk_tight_start_angle;
        const auto ball_angle           =
            std::min(std::max(0.f, std::fabs(((Field::ownGoal() - predicted_ball).normalized().toAngle() -
                                              Common::Angle::fromDeg(State::side() == -1 ? 180.f : 0.f))
                                       .deg()) -
                                   (90.f - start_ang_effect)),
                     start_ang_effect);
        const auto gk_max_dist     = penalty_area_half_width - area_notch;
        const auto ball_ang_effect =
            gk_max_dist -
            ((gk_max_dist - (Common::field().goal_width / 2.f - Common::field().robot_radius)) / start_ang_effect) *
            ball_angle;
        const float gk_target_w =
            -State::side() * std::min((Common::field().penalty_area_depth - area_notch), ball_position_effect);
        const float        gk_target_h = Common::field().penalty_area_width - 2 * area_notch;
        const Common::Vec2 gk_target_area_start{Field::ownGoal().x, -(penalty_area_half_width - area_notch)};
        Common::logDebug("ang {} pos {} pt {}", ball_angle, ball_ang_effect, penalty_area_half_width);

        const RectObstacle obs{{start, w, h}, Physicality::Virtual};

        if ((obs.inside(predicted_ball)) && (State::world().ball.velocity.length() < 1500) &&
            State::ref().running())
        {
            Common::logDebug("GK intercepting");

            m_intercepting = true;

            OldAttackerSkill{predicted_ball.angleWith(Common::Vec2(State::side() * (Common::field().width + 110), 0)),
                           0,
                           20,
                           0,
                           0,
                           false,
                           true}
                .execute(t_robot);
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
                if (intersect.distanceTo(predicted_ball) < gk_final_pos.distanceTo(predicted_ball))
                {
                    gk_final_pos = intersect;
                }
                Common::logDebug("x y : {}", intersect.x, intersect.y);
            }
            const float slope        = 0.0001538461538f;
            float       speed_effect = slope * State::world().ball.velocity.length();
            speed_effect             = std::min(speed_effect, 0.9f);
            speed_effect             = std::max(speed_effect, 0.f);
            gk_final_pos -= (gk_final_pos - Field::ownGoal()) * speed_effect;
            if (ball_angle > 0.f && (gk_final_pos - Field::ownGoal()).length() > ball_ang_effect)
            {
                gk_final_pos = (predicted_ball - Field::ownGoal()).normalized() * ball_ang_effect + Field::ownGoal();
            }
            Common::logDebug("final {} side {} un {}", gk_final_pos.x, State::side(),
                             Field::ownGoal().x - (State::side() * (Common::field().robot_radius + 20.f)));

            if (std::fabs(gk_final_pos.x * State::side()) >=
                std::fabs(Field::ownGoal().x - (State::side() * (Common::field().robot_radius + 20.f))))
            {
                gk_final_pos.x = Field::ownGoal().x - (State::side() * (Common::field().robot_radius + 20.f));
            }
            Common::debug().draw(ball_goal_line, Common::Color::red());

            t_robot.face((gk_final_pos - Field::ownGoal()).normalized() * 10000. +
                         Field::ownGoal()); // State::world().ball.position);
            t_robot.navigate(gk_final_pos, VelocityProfile::mamooli(), getNavigationFlags());
        }
    }
}

void GkTactic::shirje(Robot &t_robot)
{
    Common::logDebug("GK Shirje");

    VelocityProfile profile = VelocityProfile::kharaki();
    profile.acceleration *= 1.5f;

#if 0
    float intercept_t = -1.0f;
    float max_wait_t  = std::numeric_limits<float>::lowest();

    for (float t = 0.0f; t < 2.0f; t += 0.1f)
    {
        const Common::Vec2 point = predictBall(t).position;

        if (std::fabs(point.x) > Common::field().width)
        {
            break;
        }

        const float robot_reach_t = t_robot.calculateReachTime(point, profile);
        const float wait_t = t - robot_reach_t;

        if (wait_t > 0.0f)
        {
            break;
        }

        Common::logTrace("t: {}, wait:{}", t, wait_t);

        if (wait_t > max_wait_t)
        {
            max_wait_t  = wait_t;
            intercept_t = t;
        }
    }

    Common::logDebug("intercept t: {}", intercept_t);

    const Common::Vec2 target = predictBall(intercept_t).position;
#else
    Common::Line        ball_line  = State::world().ball.line();
    const Common::Vec2 target = ball_line.closestPoint(t_robot.state().position);
#endif

    t_robot.face(State::world().ball.position);
    //    ans = ((ans - t_robot.state().position) * 2.0f) + t_robot.state().position;
    t_robot.navigate(target, profile, getNavigationFlags());
    t_robot.chip(150);
}

NavigationFlags GkTactic::getNavigationFlags() const
{
    NavigationFlags flags = NavigationFlags::NoOwnPenaltyArea;
    if (State::ref().running())
        flags |= NavigationFlags::NoExtraMargin;
    if (State::ref().ourBallPlacement())
        flags |= NavigationFlags::BallPlacementLine;
    return flags;
}
} // namespace Tyr::Soccer
