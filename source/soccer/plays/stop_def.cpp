#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Stop_def(void)
{
    for (std::map<int *, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i)
    {
        markMap[i->first] = -1;
    }

    if (OwnRobot[attack].State.OutForSubsitute)
    {
        if (!OwnRobot[mid1].State.OutForSubsitute)
        {
            std::swap(attack, mid1);
        }
        else if (!OwnRobot[mid2].State.OutForSubsitute)
        {
            std::swap(attack, mid2);
        }
    }
    if (OwnRobot[mid1].State.Position.y < OwnRobot[mid2].State.Position.y)
    {
        std::swap(mid1, mid2);
    }

    GKHi(gk, true);
    // DefHi(def,NULL, true);
    DefMid(def, rw, lw, NULL, true);

    std::map<int, Common::Vec2> static_pos;
    static_pos[dmf]  = Common::Vec2(side * 4200, std::copysign(1000.0f, ball.Position.y));
    static_pos[mid1] = Common::Vec2(side * 4500, std::copysign(3000.0f, -ball.Position.y));
    static_pos[mid2] = Common::Vec2(side * 4200, std::copysign(1000.0f, -ball.Position.y));

    ERRTSetObstacles(dmf, true, true);
    OwnRobot[dmf].face(ball.Position);
    ERRTNavigate2Point(dmf, static_pos[dmf], 0, 40, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(mid1, true, true);
    OwnRobot[mid1].face(Common::Vec2(ball.Position.x, ball.Position.y));
    ERRTNavigate2Point(mid1, static_pos[mid1], 0, 40, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(mid2, true, true);
    OwnRobot[mid2].face(Common::Vec2(ball.Position.x, ball.Position.y));
    ERRTNavigate2Point(mid2, static_pos[mid2], 0, 40, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(attack, true, true);
    OwnRobot[attack].face(Common::Vec2(ball.Position.x, ball.Position.y));
    ERRTNavigate2Point(attack,
                       Common::Vec2(ball.Position.x, ball.Position.y)
                           .circleAroundPoint(ball.Position.angleWith(Common::Vec2(side * field_width, 0)), 580),
                       0, 40, &VELOCITY_PROFILE_AROOM);
}
} // namespace Tyr::Soccer
