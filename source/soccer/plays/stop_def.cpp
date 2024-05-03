#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Stop_def()
{
    for (std::map<int *, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i)
    {
        markMap[i->first] = -1;
    }

    if (OwnRobot[attack].State.out_for_substitute)
    {
        if (!OwnRobot[mid1].State.out_for_substitute)
        {
            std::swap(attack, mid1);
        }
        else if (!OwnRobot[mid2].State.out_for_substitute)
        {
            std::swap(attack, mid2);
        }
    }
    if (OwnRobot[mid1].State.position.y < OwnRobot[mid2].State.position.y)
    {
        std::swap(mid1, mid2);
    }

    GKHi(gk, true);
    // DefHi(def,nullptr, true);
    DefMid(def, rw, lw, nullptr, true);

    std::map<int, Common::Vec2> static_pos;
    static_pos[dmf]  = Common::Vec2(side * 4200, Common::sign(Common::worldState().ball.position.y) * 1000.0f);
    static_pos[mid1] = Common::Vec2(side * 4500, Common::sign(-Common::worldState().ball.position.y) * 3000.0f);
    static_pos[mid2] = Common::Vec2(side * 4200, Common::sign(-Common::worldState().ball.position.y) * 1000.0f);

    ERRTSetObstacles(dmf, true, true);
    OwnRobot[dmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(dmf, static_pos[dmf], 40, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(mid1, true, true);
    OwnRobot[mid1].face(Common::worldState().ball.position);
    ERRTNavigate2Point(mid1, static_pos[mid1], 40, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(mid2, true, true);
    OwnRobot[mid2].face(Common::worldState().ball.position);
    ERRTNavigate2Point(mid2, static_pos[mid2], 40, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(attack, true, true);
    OwnRobot[attack].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        attack,
        Common::worldState().ball.position.circleAroundPoint(
            Common::worldState().ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)),
            580),
        40, &VELOCITY_PROFILE_AROOM);
}
} // namespace Tyr::Soccer
