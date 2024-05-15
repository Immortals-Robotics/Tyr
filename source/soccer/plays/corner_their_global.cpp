#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::corner_their_global()
{
    if (side * m_world_state.ball.position.x > Common::field().width - 1000)
    {
        ERRTSetObstacles(gk, 0, 0);
        OwnRobot[gk].target.angle = Common::Angle::fromDeg((1 + side) * 90.0f);
        ERRTNavigate2Point(gk, Common::Vec2(side * (Common::field().width - 100), 0), 100,
                           VelocityProfile::Type::Mamooli);

        DefBy3(def, rw, lw, nullptr, true);
    }
    else
    {
        GKHi(gk, true);
        DefBy3(def, rw, lw, nullptr, true);
    }

    isDefending = true;
    DefenceWall(attack, false);

    std::map<int, Common::Vec2> static_pos;
    static_pos[dmf]  = Common::Vec2(side * 3500, Common::sign(-m_world_state.ball.position.y) * 1100.0f);
    static_pos[mid1] = Common::Vec2(side * 3200, 600);
    static_pos[mid2] = Common::Vec2(side * 3200, 0);

    MarkManager(true);

    for (std::map<int *, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i)
    {
        int opp = i->second;
        int own = *i->first;

        if (opp == -1)
        {
            if (static_pos.find(own) != static_pos.end())
            {
                OwnRobot[own].face(Common::Vec2(-side * Common::field().width, 0));
                ERRTSetObstacles(own, 1, 1);
                ERRTNavigate2Point(own, static_pos[own], 100, VelocityProfile::Type::Mamooli);
            }
        }
        else
        {
            Mark(own, opp, 500);
        }
    }
}
} // namespace Tyr::Soccer
