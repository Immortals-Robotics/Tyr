#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::normalPlayDef()
{
    Common::Vec2 ourgoal_p1 = ownGoalPostTop();
    Common::Vec2 ourgoal_p2 = ownGoalPostBottom();
    Common::debug().draw(Common::Triangle{ourgoal_p1, m_world_state.ball.position, ourgoal_p2},
                         Common::Color::blue().transparent(), true);

    m_assignments.clear();
    createGkAssignment();
    createDefAssignments();
    createMidAssignments();
    createAttackAssignment();
    assignRoles();

    for (std::map<int *, int>::const_iterator i = m_mark_map.begin(); i != m_mark_map.end(); ++i)
    {
        int opp = i->second;
        int own = *i->first;

        if (m_one_touch_detector[own].isArriving())
        {
            waitForPass(own, false);
        }
        else
        {

            if (opp == -1)
            {
                int oppAttacker = findKickerOpp(-1);

                m_own_robot[own].face(oppGoal());

                if (own == m_mid5)
                {
                    navigate(m_mid5, m_world_state.ball.position.pointOnConnectingLine(ownGoal(), 1800),
                             VelocityProfile::mamooli());
                }
                else if (own == m_mid1)
                {
                    if (oppAttacker != -1)
                        mark2Goal(own, oppAttacker, 500);
                    else
                        navigate(own, Common::Vec2(m_world_state.ball.position.x, 1000), VelocityProfile::mamooli());
                }
                else if (own == m_mid2)
                {
                    if (oppAttacker != -1)
                        mark2Goal(own, oppAttacker, 500);
                    else
                        navigate(own, Common::Vec2(m_world_state.ball.position.x, -1000), VelocityProfile::mamooli());
                }
            }
            else
            {
                mark(own, opp, 500);
            }
        }
    }

    OpenAngle     openAngle  = calculateOpenAngleToGoal(m_world_state.ball.position, m_attack);
    Common::Angle shootAngle = Common::Angle::fromDeg(180) + openAngle.center;

    float shoot_pow = 1;
    float chip_pow  = 1;
    if (m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position) > 400)
    {
        chip_pow = 1;
    }
    if (goalBlocked(m_world_state.ball.position, 200, 90))
    {
        chip_pow = 20;
    }
    else if (!goalBlocked(m_world_state.ball.position, 3000, 130))
    {
        shoot_pow = 50 - m_own_robot[m_attack].state().velocity.length() * 0.005f;
        chip_pow  = 0;
    }
    else
    {
        shoot_pow = 0;
        chip_pow  = 20;
    }

    const int kicker_opp = findKickerOpp(-1, 250.0f);

    // chip the ball out if in a dangerous position
#if 1
    if (attackFuckingAngle() && kicker_opp != -1)
    {
        shootAngle = m_world_state.ball.position.angleWith(ownGoal());
        shoot_pow  = 1;
        chip_pow   = 0;
    }
#endif

    if (kicker_opp != -1)
    {
        Common::logDebug("in m_def we trust");

        shootAngle = m_world_state.opp_robot[kicker_opp].position.angleWith(m_world_state.ball.position);
        shoot_pow  = 1;
        chip_pow   = 0;
    }

    attacker(m_attack, shootAngle, shoot_pow, chip_pow, 0, 1);
    // circleBall(m_attack, 90, 80, 0);
}
} // namespace Tyr::Soccer
