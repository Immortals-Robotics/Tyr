#include "../ai.h"

#include "../tactics/receive_pass.h"
#include "../tactics/attacker.h"

#include "../helpers/open_angle.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer;

void Ai::normalPlayAtt()
{
    Common::Vec2 oppgoal_p1 = oppGoalPostTop();
    Common::Vec2 oppgoal_p2 = oppGoalPostBottom();
    Common::debug().draw(Common::Triangle{oppgoal_p1, m_world_state.ball.position, oppgoal_p2},
                         Common::Color::red().transparent(), true);

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        Robot &robot = m_own_robot[*mid];

        Common::Vec2 static_pos = m_sorted_zones[zone_idx]->best_pos;

        const auto &allaf_pos = std::find_if(m_allaf_pos.begin(), m_allaf_pos.end(), [&robot](const auto &entry)
                                             { return *entry.first == robot.state().vision_id; });

        // allaf-pos is provided by the strategy
        if (allaf_pos != m_allaf_pos.end() && (robot.one_touch_type == Common::Soccer::OneTouchType::Allaf ||
                                               (robot.one_touch_type == Common::Soccer::OneTouchType::OneTouch &&
                                                State::timer().time().seconds() < 1.0f)))
        {
            static_pos = allaf_pos->second;
        }

        ReceivePassTactic{static_pos}.execute(robot);
        ++zone_idx;
    }

    if (m_own_robot[m_attack].one_touch_type == Common::Soccer::OneTouchType::Allaf && m_allaf_pos.contains(&m_attack))
    {
        m_own_robot[m_attack].face(oppGoal());
        m_own_robot[m_attack].navigate(m_allaf_pos[&m_attack], VelocityProfile::mamooli());
        if (m_timer.time().seconds() > 2.5)
        {
            m_own_robot[m_attack].one_touch_type = Common::Soccer::OneTouchType::OneTouch;
        }

        activeShootTimer.start();
    }

    else
    {
        OpenAngle openAngle = OpenAngle::calculateOpenAngleToGoal(m_world_state.ball.position, m_own_robot[m_attack]);

        int *suitable_mid = nullptr;
        for (const auto &mid : m_prioritized_mids)
        {
            const Robot &robot = m_own_robot[*mid];
            // const bool reached = robot.state().velocity.length() < 500;
            const bool reached = robot.state().position.distanceTo(robot.target.position) < 500;

            const bool dis_ok = robot.state().position.distanceTo(m_world_state.ball.position) > 2000;

            const bool pass_angle_ok = (robot.state().position - m_world_state.ball.position)
                                           .normalized()
                                           .dot((ownGoal() - m_world_state.ball.position).normalized()) < 0.85f;

            Common::logDebug("mid {} pass angle ok: {}", *mid, pass_angle_ok);

            const bool seen = robot.state().seen_state != Common::SeenState::CompletelyOut;

            const bool suitable = seen && reached && dis_ok && pass_angle_ok;
            if (suitable)
            {
                suitable_mid = mid;
                break;
            }
        }

        Common::logDebug("open angle: {}", openAngle.magnitude.deg());

        static bool ball_is_stationary = false;
        float       speed_threshold    = ball_is_stationary ? 1000.0f : 500.0f;
        ball_is_stationary             = m_world_state.ball.velocity.length() < speed_threshold;

        const bool opp_attacker_in_range = findKickerOpp(-1, 150.0f).has_value();

        if (openAngle.magnitude.deg() < 8 && ball_is_stationary && !opp_attacker_in_range && suitable_mid != nullptr)
        {
            Common::Angle passAngle =
                Common::Vec2(-m_side * 1700, Common::sign(-m_world_state.ball.position.y) * 1700.0f)
                    .angleWith(m_world_state.ball.position);
            float chip_pow = 40;

            if (suitable_mid)
            {
                passAngle = m_own_robot[*suitable_mid].state().position.angleWith(m_world_state.ball.position);
                chip_pow  = 15.f * m_own_robot[*suitable_mid].state().position.distanceTo(m_world_state.ball.position) /
                           8000.0f;
                chip_pow = std::min(15.f, chip_pow);
            }
            else
            {
                passAngle = oppGoal().angleWith(m_world_state.ball.position);
                chip_pow  = 0;
            }

            AttackerTactic{passAngle, 0, chip_pow, 0, 1, true}.execute(m_own_robot[m_attack]);
        }
        else
        {
            Common::Angle shootAngle;

            // target the goal center if the open angle is too small
            if (openAngle.magnitude.deg() > 5.0f)
                shootAngle = Common::Angle::fromDeg(180.0f) + openAngle.center;
            else
                shootAngle = (m_world_state.ball.position - oppGoal()).toAngle();

            float shoot_pow = 6000.f; // mm/s

            // TODO: calibrate this
            if (m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position) > 400)
            {
                shoot_pow = 1.f;
                activeShootTimer.start();
            }

            AttackerTactic{shootAngle, shoot_pow, 0, 0, 0}.execute(m_own_robot[m_attack]);
        }
    }
}
} // namespace Tyr::Soccer
