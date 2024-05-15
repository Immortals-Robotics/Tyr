#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Stop_def()
{
    for (std::map<int *, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i)
    {
        markMap[i->first] = -1;
    }

    if (OwnRobot[attack].state().out_for_substitute)
    {
        if (!OwnRobot[mid1].state().out_for_substitute)
        {
            std::swap(attack, mid1);
        }
        else if (!OwnRobot[mid2].state().out_for_substitute)
        {
            std::swap(attack, mid2);
        }
    }
    if (OwnRobot[mid1].state().position.y < OwnRobot[mid2].state().position.y)
    {
        std::swap(mid1, mid2);
    }

    GKHi(gk, true);
    DefBy3(def, rw, lw, nullptr, true);

    std::map<int, Common::Vec2> static_pos;
    static_pos[dmf]  = Common::Vec2(side * 4200, Common::sign(m_world_state.ball.position.y) * 1000.0f);
    static_pos[mid1] = Common::Vec2(side * 4500, Common::sign(-m_world_state.ball.position.y) * 3000.0f);
    static_pos[mid2] = Common::Vec2(side * 4200, Common::sign(-m_world_state.ball.position.y) * 1000.0f);

    ERRTSetObstacles(dmf, true, true);
    OwnRobot[dmf].face(m_world_state.ball.position);
    ERRTNavigate2Point(dmf, static_pos[dmf], 40, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(mid1, true, true);
    OwnRobot[mid1].face(m_world_state.ball.position);
    ERRTNavigate2Point(mid1, static_pos[mid1], 40, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(mid2, true, true);
    OwnRobot[mid2].face(m_world_state.ball.position);
    ERRTNavigate2Point(mid2, static_pos[mid2], 40, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(attack, true, true);
    OwnRobot[attack].face(m_world_state.ball.position);
    ERRTNavigate2Point(attack,
                       m_world_state.ball.position.circleAroundPoint(
                           m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)), 580),
                       40, VelocityProfile::Type::Aroom);
}
} // namespace Tyr::Soccer
