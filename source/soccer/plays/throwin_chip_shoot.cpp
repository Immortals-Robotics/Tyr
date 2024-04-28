#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::throwin_chip_shoot()
{
    // swap ( mid2 , attack );
    // swap(attack, gk);

    GKHi(gk, true);
    DefMid(def, rw, lw, NULL, false);

    /*ERRTSetObstacles ( mid2 , true , true , true , true );
    OwnRobot[mid2].face(ball.Position);
    ERRTNavigate2Point ( mid2 , Common::Vec2(side*500, -Common::sign(ball.Position.y)*1500) ,0 ,
    70,&VELOCITY_PROFILE_MAMOOLI);

    ERRTSetObstacles ( mid1 , true , true , true , true );
    OwnRobot[mid1].face(ball.Position);
    ERRTNavigate2Point ( mid1 , Common::Vec2(-side*500, -Common::sign(ball.Position.y)*1800) ,0 ,
    70,&VELOCITY_PROFILE_MAMOOLI);*/

    ERRTSetObstacles(mid2, true, true);
    OwnRobot[mid2].face(ball.Position);
    ERRTNavigate2Point(mid2, Common::Vec2(-side * 1500, Common::sign(ball.Position.y) * 2500.0f), 0, 70,
                       &VELOCITY_PROFILE_MAMOOLI);
    oneTouchType[mid2] = shirje;

    ERRTSetObstacles(mid1, true, true);
    OwnRobot[mid1].face(ball.Position);
    ERRTNavigate2Point(mid1, Common::Vec2(-side * 3500, Common::sign(ball.Position.y) * 2500.0f), 0, 70,
                       &VELOCITY_PROFILE_MAMOOLI);
    oneTouchType[mid1] = shirje;

    if (timer.time() > 4)
    {
        // tech_circle(dmf,Common::Vec2::angleWith ( Common::Vec2 ( -side*2995 , 0 ) , ball.Position ) ,0,30,0,1);
        circle_ball(dmf,
                    Common::Vec2(-side * field_width, Common::sign(ball.Position.y) * 200.0f).angleWith(ball.Position),
                    0, 15, 1.0f);
    }
    else
    {
        // tech_circle(dmf,Common::Vec2::angleWith ( Common::Vec2 ( -side*2995 , 0 ) , ball.Position ) ,0,0,0,1);
        circle_ball(dmf, Common::Vec2(-side * field_width, 0).angleWith(ball.Position), 0, 0, 1.0f);
    }

    OwnRobot[attack].face(Common::Vec2(-side * field_width, 0));
    ERRTSetObstacles(attack, 0, 1); // TODO the Obstacle avoidance for Opp was disabled (just added it)
    AddCircle(ball.Position.x, ball.Position.y, 320);
    if (randomParam < 0.0)
        ERRTNavigate2Point(
            attack, ball.Position.pointOnConnectingLine(Common::Vec2(-side * field_width, 0), 350 + 700 * reached));
    else if (randomParam < 0.5)
        ERRTNavigate2Point(attack, ball.Position.pointOnConnectingLine(
                                       Common::Vec2(-side * field_width, Common::sign(-ball.Position.x) * 2000.0f),
                                       350 + 700 * reached));
    else
        ERRTNavigate2Point(attack, ball.Position.pointOnConnectingLine(
                                       Common::Vec2(-side * field_width, Common::sign(ball.Position.x) * 2000.0f),
                                       350 + 700 * reached));

    // swap(attack, gk);
    // swap ( mid2 , attack );
}
} // namespace Tyr::Soccer