#include "../ai.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer;

void Ai::normalPlayAtt()
{
    Common::Vec2 oppgoal_p1 = oppGoalPostTop();
    Common::Vec2 oppgoal_p2 = oppGoalPostBottom();
    Common::debug().draw(Common::Triangle{oppgoal_p1, m_world_state.ball.position, oppgoal_p2},
                         Common::Color::red().transparent(), true);

    receivePass(m_mid5, m_sorted_zones[0]->best_pos);
    receivePass(m_mid1, m_sorted_zones[1]->best_pos);
    receivePass(m_mid2, m_sorted_zones[2]->best_pos);

    if (m_one_touch_type[m_attack] == OneTouchType::Allaf && m_allaf_pos.contains(&m_attack))
    {
        m_own_robot[m_attack].face(oppGoal());
        navigate(m_attack, m_allaf_pos[&m_attack], VelocityProfile::mamooli());
        if (m_timer.time().seconds() > 2.5)
        {
            m_one_touch_type[m_attack] = OneTouchType::OneTouch;
        }

        activeShootTimer.start();
    }

    else
    {
        OpenAngle openAngle = calculateOpenAngleToGoal(m_world_state.ball.position, m_attack);

        int *suitable_mid = nullptr;
        for (const auto &mid : m_prioritized_mids)
        {
            const Robot &robot = m_own_robot[*mid];
            // const bool reached = robot.state().velocity.length() < 500;
            const bool reached = robot.state().position.distanceTo(robot.target.position) < 500;

            const bool dis_ok = robot.state().position.distanceTo(m_world_state.ball.position) > 2000;

            const bool pass_angle_ok = (robot.state().position - m_world_state.ball.position)
                                           .normalized()
                                           .dot((ownGoal() - m_world_state.ball.position).normalized()) < 0.75f;

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
        if (openAngle.magnitude.deg() < 8 && (findKickerOpp(-1, 500.0f) == -1) && (suitable_mid != nullptr))
        {
            Common::Angle passAngle =
                Common::Vec2(-m_side * 1700, Common::sign(-m_world_state.ball.position.y) * 1700.0f)
                    .angleWith(m_world_state.ball.position);
            float chip_pow = 40;

            if (suitable_mid)
            {
                passAngle = m_own_robot[*suitable_mid].state().position.angleWith(m_world_state.ball.position);
                chip_pow  = 50.f * m_own_robot[*suitable_mid].state().position.distanceTo(m_world_state.ball.position) /
                           4000.0f;
                chip_pow = std::min(50.f, chip_pow);
            }
            else
            {
                passAngle = oppGoal().angleWith(m_world_state.ball.position);
                chip_pow  = 0;
            }
            
            attacker(m_attack, passAngle, 0, chip_pow, 0, 1);
        }
        else
        {

            Common::Angle shootAngle;
            shootAngle = Common::Angle::fromDeg(180.0f) + openAngle.center;

            float shoot_pow = 50 - m_own_robot[m_attack].state().velocity.length() * 0.005;

            if (m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position) > 400)
            {
                shoot_pow = 1;
                activeShootTimer.start();
            }
            else if (goalBlocked(m_world_state.ball.position, 200, 90))
            {
                shoot_pow = 1;
            }

            attacker(m_attack, shootAngle, shoot_pow, 0, 0, 0);
        }
    }
}
} // namespace Tyr::Soccer
