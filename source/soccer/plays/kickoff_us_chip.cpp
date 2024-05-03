#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoff_us_chip()
{
    GKHi(gk, true);
    DefMid(def, rw, lw, nullptr, false);

    ERRTSetObstacles(dmf, true, true);
    OwnRobot[dmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        dmf,
        Common::worldState().ball.position.pointOnConnectingLine(
            Common::Vec2(side * Common::worldState().field.width, 0),
            Common::worldState().ball.position.distanceTo(Common::Vec2(side * Common::worldState().field.width, 0)) /
                3.0f),
        40, &VELOCITY_PROFILE_MAMOOLI);

    if (timer.time() < 0.5)
    {
        if (OwnRobot[mid1].State.position.y > OwnRobot[mid2].State.position.y)
        {
            std::swap(mid1, mid2);
        }
    }

    OwnRobot[mid2].face(Common::Vec2(-side * Common::worldState().field.width, 0));
    ERRTSetObstacles(mid2, true, true);
    ERRTNavigate2Point(mid2, Common::Vec2(Common::worldState().ball.position.x + side * 150,
                                          (Common::worldState().field.height - 300)));
    OwnRobot[mid1].face(Common::Vec2(-side * Common::worldState().field.width, 0));
    ERRTSetObstacles(mid1, true, true);
    ERRTNavigate2Point(mid1, Common::Vec2(Common::worldState().ball.position.x + side * 150,
                                          -(Common::worldState().field.height - 300)));
    Common::Vec2 chip_target = Common::Vec2(-side * 2000, 0);
    if (Common::refereeState().canKickBall())
    {
        tech_circle(attack, chip_target.angleWith(Common::worldState().ball.position), 0, 80, 0, 1, 0, 1);
        // circle_ball(attack, Common::Vec2::angleWith ( chip_target , Common::worldState().ball.position ), 100,
        // 0, 1.0f);
        std::cout << "IN THE IFFFFFFFF!!!" << std::endl;
    }
    else
    {
        // tech_circle(attack,Common::Vec2::angleWith ( chip_target , Common::worldState().ball.position ),0,0,0,1,0,1);
        circle_ball(attack, chip_target.angleWith(Common::worldState().ball.position), 0, 0, 1.0f);
        std::cout << "IN THE ELSE!!!" << std::endl;
    }
}
} // namespace Tyr::Soccer
