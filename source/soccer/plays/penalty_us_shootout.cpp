#include "../ai.h"

namespace Tyr::Soccer
{
static float t_nml;

void Ai::penalty_us_shootout()
{
    DefMid(def, rw, lw, nullptr, true);

    OwnRobot[dmf].face(Common::Vec2(-side * Common::field().width, 0));
    ERRTSetObstacles(dmf, true, true);
    ERRTNavigate2Point(dmf, Common::Vec2(side * 4000, 0), 80, &VELOCITY_PROFILE_AROOM);

    OwnRobot[mid1].face(Common::Vec2(-side * Common::field().width, 0));
    ERRTSetObstacles(mid1, true, true);
    ERRTNavigate2Point(mid1, Common::Vec2(side * 4000, -500), 80, &VELOCITY_PROFILE_AROOM);

    OwnRobot[mid2].face(Common::Vec2(-side * Common::field().width, 0));
    ERRTSetObstacles(mid2, true, true);
    ERRTNavigate2Point(mid2, Common::Vec2(side * 4000, 500), 80, &VELOCITY_PROFILE_AROOM);

    if (!Common::refereeState().canKickBall())
    {
        Common::logInfo("step0 - Waiting for permission");
    }
    else if (Common::worldState().ball.position.distanceTo(Common::Vec2(-side * Common::field().width, 0)) > 3000)
    {
        circle_ball(attack, Common::Vec2(-side * Common::field().width, 0).angleWith(Common::worldState().ball.position), 1, 0,
                    0);
        Common::logInfo("step1 - Moving forward - waiting to get close to the opp goal");
    }
    else
    {
        circle_ball(attack, Common::Vec2(-side * Common::field().width, 400).angleWith(Common::worldState().ball.position), 60, 0,
                    0);
        Common::logInfo("step2 - Kick in the goal!!!!");
    }
}
} // namespace Tyr::Soccer