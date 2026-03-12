#include "../ai.h"

#include "../tactics/receive_pass.h"
#include "../tactics/attacker.h"

#include "../helpers/find_kicker_opp.h"
#include "../helpers/open_angle.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer;

void Ai::normalPlayAtt()
{
    Common::Vec2 oppgoal_p1 = Field::oppGoalPostTop();
    Common::Vec2 oppgoal_p2 = Field::oppGoalPostBottom();
    Common::debug().draw(Common::Triangle{oppgoal_p1, m_world_state.ball.position, oppgoal_p2},
                         Common::Color::red().transparent(), true);

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        Robot &robot = m_own_robot[*mid];

        Common::Vec2 static_pos = m_sorted_zones[zone_idx]->best_pos;

        const auto &allaf_pos = std::find_if(m_allaf_pos.begin(), m_allaf_pos.end(), [&robot](const auto &entry)
        {
            return *entry.first == robot.state().vision_id;
        });

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

    OpenAngle openAngle = OpenAngle::calculateOpenAngleToGoal(m_world_state.ball.position, m_own_robot[m_attack]);

    int *suitable_mid = nullptr;
    float max_score = std::numeric_limits<float>::lowest();
    for (const auto &mid : m_prioritized_mids)
    {
        const Robot &robot = m_own_robot[*mid];
        // const bool reached = robot.state().velocity.length() < 500;
        const bool reached = robot.state().position.distanceTo(robot.target.position) < 1000;

        const bool dis_ok = robot.state().position.distanceTo(m_world_state.ball.position) > 1000;

        const bool pass_angle_ok = true || (robot.state().position - m_world_state.ball.position)
                                   .normalized()
                                   .dot((Field::ownGoal() - m_world_state.ball.position).normalized()) < 0.85f;



        const bool seen = robot.state().seen_state != Common::SeenState::CompletelyOut;

        const Common::Config::RobotPhysicalStatus& physical = Common::config().soccer.robot_physical_status[robot.state().vision_id];
        const bool abilities = physical.has_direct_kick && !physical.is_3D_printed;

        const bool suitable = seen && reached && dis_ok && pass_angle_ok && abilities;
        if (!suitable)
        {
            continue;
        }

        const auto score = staticPosScoreAttack(robot.target.position);
        if (score > max_score)
        {
            max_score = score;
            suitable_mid = mid;
        }
    }

    Common::logDebug("suitable_mid {}", suitable_mid ? *suitable_mid : -1);
    Common::logDebug("open angle: {}", openAngle.magnitude.deg());

    static bool ball_is_stationary = false;
    float       speed_threshold    = ball_is_stationary ? 1000.0f : 500.0f;
    ball_is_stationary             = m_world_state.ball.velocity.length() < speed_threshold;

    const bool opp_attacker_in_range = findKickerOpp(-1, 150.0f).has_value();

    static int kick_pass_hys = 0;
    const int max_hys = static_cast<int>(Common::config().vision.vision_frame_rate * 0.5);

    if (m_timer.time().seconds() < 0.5) {
        kick_pass_hys = 0;
    }

    if (openAngle.magnitude.deg() < 15 && !opp_attacker_in_range && suitable_mid != nullptr)
    {
        if (kick_pass_hys == 0)
        {
            kick_pass_hys = max_hys;
        }
        else
        {
            kick_pass_hys = std::min(kick_pass_hys + 1, max_hys);
        }
    }
    else
    {
        if (kick_pass_hys == 0)
        {
            kick_pass_hys = -max_hys;
        }
        else
        {
            kick_pass_hys = std::max(kick_pass_hys - 1, -max_hys);
        }
    }

    if (suitable_mid == nullptr) {
        kick_pass_hys = -max_hys;
    }

    Common::logDebug("kick_pass_hys {}", kick_pass_hys);

    if (kick_pass_hys > 0)
    {
        Common::Angle passAngle =
            Common::Vec2(-m_side * 1700, Common::sign(-m_world_state.ball.position.y) * 1700.0f)
            .angleWith(m_world_state.ball.position);

        const Common::Vec2 target_pos = m_own_robot[*suitable_mid].target.position;
        float chip_pow = 40;

        passAngle = target_pos.angleWith(m_world_state.ball.position);
        chip_pow  = 100.f * target_pos.distanceTo(m_world_state.ball.position) /
                   8000.0f;
        chip_pow = std::clamp(chip_pow, 15.0f, 100.f);
        float kick_power = 5000.0f;

        if (shootBlocked(m_world_state.ball.position, target_pos, 2000.0, 150.0)) {
            kick_power = 0.0f;
        } else {
            chip_pow = 0.0f;
        }

        AttackerTactic{passAngle, false, kick_power, chip_pow}.execute(m_own_robot[m_attack]);
    }
    else
    {
        Common::Angle shootAngle;

        // target the goal center if the open angle is too small
        Common::logCritical("Open angle magnitude {}", openAngle.magnitude.deg());
        if (openAngle.magnitude.deg() > 1.0f)
            shootAngle = Common::Angle::fromDeg(180.0f) + openAngle.center;
        else
            shootAngle = (m_world_state.ball.position - Field::oppGoal()).toAngle();

        float shoot_pow = 6000.f; // mm/s

        // TODO: calibrate this
        if (m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position) > 400)
        {
            shoot_pow = 1.f;
            activeShootTimer.start();
        }

        AttackerTactic{shootAngle, true, shoot_pow, 0}.execute(m_own_robot[m_attack]);
    }

}
} // namespace Tyr::Soccer
