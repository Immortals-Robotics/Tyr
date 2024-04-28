#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::corner_their_global()
{
    if (side * ball.Position.x > field_width - 1000)
    {
        ERRTSetObstacles(gk, 0, 0);
        OwnRobot[gk].target.angle = Common::Angle::fromDeg((1 + side) * 90.0f);
        ERRTNavigate2Point(gk, Common::vec2(side * (field_width - 100), 0), 0, 100, &VELOCITY_PROFILE_MAMOOLI);

        DefMid(def, rw, lw, NULL, false);
    }
    else
    {
        GKHi(gk, true);
        DefMid(def, rw, lw, NULL, false);
    }

    isDefending = true;
    DefenceWall(attack, false);

    std::map<int, Common::vec2> static_pos;
    static_pos[dmf]  = Common::vec2(side * 3500, std::copysign(1100.0f, -ball.Position.y));
    static_pos[mid1] = Common::vec2(side * 3200, 600);
    static_pos[mid2] = Common::vec2(side * 3200, 0);

    MarkManager(true);

    for (std::map<int *, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i)
    {
        int opp = i->second;
        int own = *i->first;

        if (opp == -1)
        {
            if (static_pos.find(own) != static_pos.end())
            {
                OwnRobot[own].face(Common::vec2(-side * field_width, 0));
                ERRTSetObstacles(own, 1, 1);
                ERRTNavigate2Point(own, static_pos[own], 0, 100, &VELOCITY_PROFILE_MAMOOLI);
            }
        }
        else
        {
            Mark(own, opp, 500);
        }
    }
}
} // namespace Tyr::Soccer
