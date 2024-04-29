#include "../ai.h"

namespace Tyr::Soccer
{
static float t_nml;

void Ai::penalty_us_shootout()
{

    bool canKickBall = bool(currentPlayParam);
    if (!canKickBall)
    {
    }

    DefMid(def, rw, lw, NULL, true);

    OwnRobot[dmf].face(Common::Vec2(-side * field_width, 0));
    ERRTSetObstacles(dmf, true, true);
    ERRTNavigate2Point(dmf, Common::Vec2(side * 4000, 0), false, 80, &VELOCITY_PROFILE_AROOM);

    OwnRobot[mid1].face(Common::Vec2(-side * field_width, 0));
    ERRTSetObstacles(mid1, true, true);
    ERRTNavigate2Point(mid1, Common::Vec2(side * 4000, -500), false, 80, &VELOCITY_PROFILE_AROOM);

    OwnRobot[mid2].face(Common::Vec2(-side * field_width, 0));
    ERRTSetObstacles(mid2, true, true);
    ERRTNavigate2Point(mid2, Common::Vec2(side * 4000, 500), false, 80, &VELOCITY_PROFILE_AROOM);

    if (!canKickBall)
    {
        Common::logInfo("step0 - Waiting for permission");
    }
    else if (ball.Position.distanceTo(Common::Vec2(-side * field_width, 0)) > 3000)
    {
        circle_ball(attack, Common::Vec2(-side * field_width, 0).angleWith(ball.Position), 1, 0, 0);
        Common::logInfo("step1 - Moving forward - waiting to get close to the opp goal");
    }
    else
    {
        circle_ball(attack, Common::Vec2(-side * field_width, 400).angleWith(ball.Position), 60, 0, 0);
        Common::logInfo("step2 - Kick in the goal!!!!");
    }
}
} // namespace Tyr::Soccer