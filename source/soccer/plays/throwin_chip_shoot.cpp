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
    ERRTNavigate2Point ( mid2 , Common::vec2(side*500, -sgn(ball.Position.y)*1500) ,0 , 70,&VELOCITY_PROFILE_MAMOOLI);

    ERRTSetObstacles ( mid1 , true , true , true , true );
    OwnRobot[mid1].face(ball.Position);
    ERRTNavigate2Point ( mid1 , Common::vec2(-side*500, -sgn(ball.Position.y)*1800) ,0 ,
    70,&VELOCITY_PROFILE_MAMOOLI);*/

    ERRTSetObstacles(mid2, true, true);
    OwnRobot[mid2].face(ball.Position);
    ERRTNavigate2Point(mid2, Common::vec2(-side * 1500, std::copysign(2500.0f, ball.Position.y)), 0, 70,
                       &VELOCITY_PROFILE_MAMOOLI);
    oneTouchType[mid2] = shirje;

    ERRTSetObstacles(mid1, true, true);
    OwnRobot[mid1].face(ball.Position);
    ERRTNavigate2Point(mid1, Common::vec2(-side * 3500, std::copysign(2500.0f, ball.Position.y)), 0, 70,
                       &VELOCITY_PROFILE_MAMOOLI);
    oneTouchType[mid1] = shirje;

    if (timer.time() > 4)
    {
        // tech_circle(dmf,Common::angle_with ( Common::vec2 ( -side*2995 , 0 ) , ball.Position ) ,0,30,0,1);
        circle_ball(dmf,
                    Common::angle_with(Common::vec2(-side * field_width, std::copysign(200.0f, ball.Position.y)),
                                       ball.Position),
                    0, 15, 1.0f);
    }
    else
    {
        // tech_circle(dmf,Common::angle_with ( Common::vec2 ( -side*2995 , 0 ) , ball.Position ) ,0,0,0,1);
        circle_ball(dmf, Common::angle_with(Common::vec2(-side * field_width, 0), ball.Position), 0, 0, 1.0f);
    }

    OwnRobot[attack].face(Common::vec2(-side * field_width, 0));
    ERRTSetObstacles(attack, 0, 1); // TODO the Obstacle avoidance for Opp was disabled (just added it)
    AddCircle(ball.Position.x, ball.Position.y, 320);
    if (randomParam < 0.0)
        ERRTNavigate2Point(attack, Common::point_on_connecting_line(ball.Position, Common::vec2(-side * field_width, 0),
                                                                 350 + 700 * reached));
    else if (randomParam < 0.5)
        ERRTNavigate2Point(attack, Common::point_on_connecting_line(
                                       ball.Position,
                                       Common::vec2(-side * field_width, std::copysign(2000.0f, -ball.Position.x)),
                                       350 + 700 * reached));
    else
        ERRTNavigate2Point(attack, Common::point_on_connecting_line(
                                       ball.Position,
                                       Common::vec2(-side * field_width, std::copysign(2000.0f, ball.Position.x)),
                                       350 + 700 * reached));

    // swap(attack, gk);
    // swap ( mid2 , attack );
}
} // namespace Tyr::Soccer