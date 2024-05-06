#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::throwin_chip_shoot()
{
    GKHi(gk, true);
    DefMid(def, rw, lw, nullptr, false);

    ERRTSetObstacles(mid2, true, true);
    OwnRobot[mid2].face(Common::worldState().ball.position);
    ERRTNavigate2Point(mid2, Common::Vec2(-side * 1500, Common::sign(Common::worldState().ball.position.y) * 2500.0f),
                       70, &VELOCITY_PROFILE_MAMOOLI);
    oneTouchType[mid2] = shirje;

    ERRTSetObstacles(mid1, true, true);
    OwnRobot[mid1].face(Common::worldState().ball.position);
    ERRTNavigate2Point(mid1, Common::Vec2(-side * 3500, Common::sign(Common::worldState().ball.position.y) * 2500.0f),
                       70, &VELOCITY_PROFILE_MAMOOLI);
    oneTouchType[mid1] = shirje;

    if (timer.time() > 4)
    {
        // tech_circle(dmf,Common::Vec2::angleWith ( Common::Vec2 ( -side*2995 , 0 ) ,
        // Common::worldState().ball.position ) ,0,30,0,1);
        circle_ball(dmf,
                    Common::Vec2(-side * Common::field().width,
                                 Common::sign(Common::worldState().ball.position.y) * 200.0f)
                        .angleWith(Common::worldState().ball.position),
                    0, 15, 1.0f);
    }
    else
    {
        // tech_circle(dmf,Common::Vec2::angleWith ( Common::Vec2 ( -side*2995 , 0 ) ,
        // Common::worldState().ball.position ) ,0,0,0,1);
        circle_ball(
            dmf,
            Common::Vec2(-side * Common::field().width, 0).angleWith(Common::worldState().ball.position), 0,
            0, 1.0f);
    }

    OwnRobot[attack].face(Common::Vec2(-side * Common::field().width, 0));
    ERRTSetObstacles(attack, 0, 1); // TODO the Obstacle avoidance for Opp was disabled (just added it)
    obs_map.addCircle({Common::worldState().ball.position, 320.0f});
    if (randomParam < 0.0)
        ERRTNavigate2Point(attack, Common::worldState().ball.position.pointOnConnectingLine(
                                       Common::Vec2(-side * Common::field().width, 0), 350));
    else if (randomParam < 0.5)
        ERRTNavigate2Point(attack, Common::worldState().ball.position.pointOnConnectingLine(
                                       Common::Vec2(-side * Common::field().width,
                                                    Common::sign(-Common::worldState().ball.position.x) * 2000.0f),
                                       350));
    else
        ERRTNavigate2Point(attack, Common::worldState().ball.position.pointOnConnectingLine(
                                       Common::Vec2(-side * Common::field().width,
                                                    Common::sign(Common::worldState().ball.position.x) * 2000.0f),
                                       350));
}
} // namespace Tyr::Soccer