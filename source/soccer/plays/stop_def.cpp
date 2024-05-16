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
    DefHi(def, rw, lw, nullptr, true);

    std::map<int, Common::Vec2> static_pos;
    static_pos[dmf]  = Common::Vec2(side * 4200, Common::sign(m_world_state.ball.position.y) * 1000.0f);
    static_pos[mid1] = Common::Vec2(side * 4500, Common::sign(-m_world_state.ball.position.y) * 3000.0f);
    static_pos[mid2] = Common::Vec2(side * 4200, Common::sign(-m_world_state.ball.position.y) * 1000.0f);

    OwnRobot[dmf].face(m_world_state.ball.position);
    navigate(dmf, static_pos[dmf], VelocityProfile::aroom());

    OwnRobot[mid1].face(m_world_state.ball.position);
    navigate(mid1, static_pos[mid1], VelocityProfile::aroom());

    OwnRobot[mid2].face(m_world_state.ball.position);
    navigate(mid2, static_pos[mid2], VelocityProfile::aroom());

    OwnRobot[attack].face(m_world_state.ball.position);
    navigate(attack,
             m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 580),
             VelocityProfile::aroom());
}
} // namespace Tyr::Soccer
