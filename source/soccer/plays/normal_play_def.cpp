#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::NormalPlayDef()
{
    Common::Vec2 ourgoal_p1 = ownGoalPostTop();
    Common::Vec2 ourgoal_p2 = ownGoalPostBottom();
    Common::debug().draw(Common::Triangle{ourgoal_p1, m_world_state.ball.position, ourgoal_p2},
                         Common::Color::blue().transparent(), true);

    ManageAttRoles();

    MarkManager(false);

    for (std::map<int *, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i)
    {
        int opp = i->second;
        int own = *i->first;

        if (oneTouchDetector[own].IsArriving())
        {
            WaitForPass(own, false);
        }
        else
        {

            if (opp == -1)
            {
                int oppAttacker = findKickerOpp(-1);

                OwnRobot[own].face(oppGoal());

                if (own == dmf)
                {
                    navigate(dmf, m_world_state.ball.position.pointOnConnectingLine(ownGoal(), 1800),
                             VelocityProfile::mamooli());
                }
                else if (own == mid1)
                {
                    if (oppAttacker != -1)
                        Mark2Goal(own, oppAttacker, 500);
                    else
                        navigate(own, Common::Vec2(m_world_state.ball.position.x, 1000), VelocityProfile::mamooli());
                }
                else if (own == mid2)
                {
                    if (oppAttacker != -1)
                        Mark2Goal(own, oppAttacker, 500);
                    else
                        navigate(own, Common::Vec2(m_world_state.ball.position.x, -1000), VelocityProfile::mamooli());
                }
            }
            else
            {
                Mark(own, opp, 500);
            }
        }
    }

    OpenAngle     openAngle  = calculateOpenAngleToGoal(m_world_state.ball.position, attack);
    Common::Angle shootAngle = Common::Angle::fromDeg(180) + openAngle.center;

    float shoot_pow = 1;
    float chip_pow  = 1;
    if (OwnRobot[attack].state().position.distanceTo(m_world_state.ball.position) > 400)
    {
        chip_pow = 1;
    }
    if (goalBlocked(m_world_state.ball.position, 200, 90))
    {
        chip_pow = 20;
    }
    else if (!goalBlocked(m_world_state.ball.position, 3000, 130))
    {
        shoot_pow = 50 - OwnRobot[attack].state().velocity.length() * 0.005f;
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
        Common::logDebug("in def we trust");

        shootAngle = m_world_state.opp_robot[kicker_opp].position.angleWith(m_world_state.ball.position);
        shoot_pow  = 1;
        chip_pow   = 0;
    }

    tech_circle(attack, shootAngle, shoot_pow, chip_pow, 0, 0, 1);
    // circle_ball(attack, 90, 80, 0, 1.0f);
}
} // namespace Tyr::Soccer
