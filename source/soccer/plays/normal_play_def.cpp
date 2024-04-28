#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::NormalPlayDef(void)
{
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

                OwnRobot[own].face(Common::Vec2(-side * field_width, 0));
                ERRTSetObstacles(own, 0, 1);

                if (own == dmf)
                {
                    ERRTNavigate2Point(dmf,
                                       ball.Position.pointOnConnectingLine(Common::Vec2(side * field_width, 0), 1800),
                                       0, 100, &VELOCITY_PROFILE_MAMOOLI);
                }
                else if (own == mid1)
                {
                    if (oppAttacker != -1)
                        Mark2Goal(own, oppAttacker, 500);
                    else
                        ERRTNavigate2Point(own, Common::Vec2(ball.Position.x, 1000), 0, 100, &VELOCITY_PROFILE_MAMOOLI);
                }
                else if (own == mid2)
                {
                    if (oppAttacker != -1)
                        Mark2Goal(own, oppAttacker, 500);
                    else
                        ERRTNavigate2Point(own, Common::Vec2(ball.Position.x, -1000), 0, 100,
                                           &VELOCITY_PROFILE_MAMOOLI);
                }
            }
            else
            {
                Mark(own, opp, 500);
            }
        }
    }

    Common::Vec2  openAngle  = calculateOpenAngleToGoal(ball.Position, attack);
    Common::Angle shootAngle = Common::Angle::fromDeg(180 + openAngle.x);

    float shoot_pow = 1;
    float chip_pow  = 1;
    if (OwnRobot[attack].State.Position.distanceTo(ball.Position) > 400)
    {
        chip_pow = 1;
    }
    if (goal_blocked(ball.Position, 200, 90))
    {
        chip_pow = 20;
    }
    else if (!goal_blocked(ball.Position, 3000, 130))
    {
        shoot_pow = 50 - OwnRobot[attack].State.velocity.length() * 0.005f;
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
        shootAngle = ball.Position.angleWith(Common::Vec2(side * field_width, 0));
        shoot_pow  = 1;
        chip_pow   = 0;
    }
#endif

    if (kicker_opp != -1)
    {
        LOG_DEBUG("in def we trust");

        shootAngle = OppRobot[kicker_opp].Position.angleWith(ball.Position);
        shoot_pow  = 1;
        chip_pow   = 0;
    }

    debugDraw = true;
    tech_circle(attack, shootAngle, shoot_pow, chip_pow, 1, 0, 0, 1);
    // circle_ball(attack, 90, 80, 0, 1.0f);
    debugDraw = false;
}
} // namespace Tyr::Soccer
