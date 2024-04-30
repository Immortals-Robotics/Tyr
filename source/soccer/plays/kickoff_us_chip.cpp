#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoff_us_chip(void)
{
    bool canKickBall = bool(currentPlayParam);
    GKHi(gk, true);
    DefMid(def, rw, lw, NULL, false);

    ERRTSetObstacles(dmf, true, true);
    OwnRobot[dmf].face(ball.Position);
    ERRTNavigate2Point(
        dmf,
        ball.Position.pointOnConnectingLine(Common::Vec2(side * field_width, 0),
                                            ball.Position.distanceTo(Common::Vec2(side * field_width, 0)) / 3.0f),
        0, 40, &VELOCITY_PROFILE_MAMOOLI);

    if (timer.time() < 0.5)
    {
        if (OwnRobot[mid1].State.Position.y > OwnRobot[mid2].State.Position.y)
        {
            std::swap(mid1, mid2);
        }
    }

    OwnRobot[mid2].face(Common::Vec2(-side * field_width, 0));
    ERRTSetObstacles(mid2, true, true);
    ERRTNavigate2Point(mid2, Common::Vec2(ball.Position.x + side * 150, (field_height - 300)));
    OwnRobot[mid1].face(Common::Vec2(-side * field_width, 0));
    ERRTSetObstacles(mid1, true, true);
    ERRTNavigate2Point(mid1, Common::Vec2(ball.Position.x + side * 150, -(field_height - 300)));
    Common::Vec2 chip_target = Common::Vec2(-side * 2000, 0);
    if (canKickBall)
    {
        tech_circle(attack, chip_target.angleWith(ball.Position), 0, 80, 0, 1, 0, 1);
        // circle_ball(attack, Common::Vec2::angleWith ( chip_target , ball.Position ), 100, 0, 1.0f);
        std::cout << "IN THE IFFFFFFFF!!!" << std::endl;
    }
    else
    {
        // tech_circle(attack,Common::Vec2::angleWith ( chip_target , ball.Position ),0,0,0,1,0,1);
        circle_ball(attack, chip_target.angleWith(ball.Position), 0, 0, 1.0f);
        std::cout << "IN THE ELSE!!!" << std::endl;
    }
}
} // namespace Tyr::Soccer