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

        bool mid1Reached = m_own_robot[m_mid1].state().velocity.length() < 500;
        bool mid2Reached = m_own_robot[m_mid2].state().velocity.length() < 500;

        bool mid1DisOk = m_own_robot[m_mid1].state().position.distanceTo(m_world_state.ball.position) > 2000;
        bool mid2DisOk = m_own_robot[m_mid2].state().position.distanceTo(m_world_state.ball.position) > 2000;

        bool mid1PassAngleOk = (m_own_robot[m_mid1].state().position - m_world_state.ball.position)
                                   .normalized()
                                   .dot((ownGoal() - m_world_state.ball.position).normalized()) < 0.75f;

        bool mid2PassAngleOk = (m_own_robot[m_mid2].state().position - m_world_state.ball.position)
                                   .normalized()
                                   .dot((ownGoal() - m_world_state.ball.position).normalized()) < 0.75f;

        Common::logDebug("pass angle ok m_mid1 : {}, m_mid2: {}", mid1PassAngleOk, mid2PassAngleOk);

        bool mid1Seen = m_own_robot[m_mid1].state().seen_state != Common::SeenState::CompletelyOut;
        bool mid2Seen = m_own_robot[m_mid2].state().seen_state != Common::SeenState::CompletelyOut;

        bool mid1Suitable = mid1Seen && mid1Reached && mid1DisOk && mid1PassAngleOk;
        bool mid2Suitable = mid2Seen && mid2Reached && mid2DisOk && mid2PassAngleOk;

        if (mid1Suitable && mid2Suitable)
        {
            if (-m_side * m_own_robot[m_mid1].state().position.x > -m_side * m_own_robot[m_mid2].state().position.x)
                mid2Suitable = false;
            else
                mid1Suitable = false;
        }

        Common::logDebug("open angle: {}", openAngle.magnitude.deg());
        if (openAngle.magnitude.deg() < 8 && (findKickerOpp(-1, 500.0f) == -1) && (mid1Suitable || mid2Suitable))
        {

            Common::Angle passAngle =
                Common::Vec2(-m_side * 1700, Common::sign(-m_world_state.ball.position.y) * 1700.0f)
                    .angleWith(m_world_state.ball.position);
            float chip_pow = 40;

            if (mid1Suitable)
            {
                passAngle = m_own_robot[m_mid1].state().position.angleWith(m_world_state.ball.position);
                chip_pow =
                    50.f * m_own_robot[m_mid1].state().position.distanceTo(m_world_state.ball.position) / 4000.0f;
                chip_pow = std::min(50.f, chip_pow);
            }
            else if (mid2Suitable)
            {
                passAngle = m_own_robot[m_mid2].state().position.angleWith(m_world_state.ball.position);
                chip_pow =
                    50.f * m_own_robot[m_mid2].state().position.distanceTo(m_world_state.ball.position) / 4000.0f;
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
