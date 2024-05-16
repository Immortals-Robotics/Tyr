#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Stop()
{
#if !mark_in_stop
    for (std::map<int *, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i)
    {
        markMap[i->first] = -1;
    }
#endif

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
    if (side * OwnRobot[mid1].state().position.y < side * OwnRobot[mid2].state().position.y) // Nice :D
    {
        std::swap(mid1, mid2);
    }

    GKHi(gk, true);
    DefHi(def, rw, lw, nullptr, true);

#if mark_in_stop
    MarkManager(false);

    for (std::map<int *, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i)
    {
        int opp = i->second;
        int own = *i->first;

        if (opp == -1)
        {
            if (own == dmf)
            {
                setObstacles(dmf);
                OwnRobot[dmf].face(m_world_state.ball.position);
                navigate(dmf,
                         pointOnConnectingLine(m_world_state.ball.position,
                                               ownGoal(),
                                               Common::Vec2::distance(m_world_state.ball.position,
                                                                      ownGoal()) /
                                                   3.0f),
                         VelocityProfile::aroom());
                OwnRobot[dmf].shoot(0);
            }
            else if (own == mid2)
            {
                setObstacles(mid2);
                OwnRobot[mid2].face(m_world_state.ball.position);
                navigate(mid2,
                         circleAroundPoint(m_world_state.ball.position,
                                           NormalizeAngle(-20 + Common::Vec2::angleWith(
                                                                    m_world_state.ball.position,
                                                                    ownGoal())),
                                           650),
                         VelocityProfile::aroom());
                OwnRobot[mid2].shoot(0);
            }
            else if (own == mid1)
            {
                setObstacles(mid1);
                OwnRobot[mid1].face(m_world_state.ball.position);
                navigate(mid1,
                         circleAroundPoint(m_world_state.ball.position,
                                           NormalizeAngle(20 + Common::Vec2::angleWith(
                                                                   m_world_state.ball.position,
                                                                   ownGoal())),
                                           650),
                         VelocityProfile::aroom());
                OwnRobot[mid1].shoot(0);
            }
        }
        else
        {
            Mark(own, opp, 500);
        }
    }
#else
    setObstacles(dmf);
    OwnRobot[dmf].face(m_world_state.ball.position);
    navigate(dmf,
             m_world_state.ball.position.pointOnConnectingLine(
                 ownGoal(),
                 m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
             VelocityProfile::aroom());
    OwnRobot[dmf].shoot(0);

    setObstacles(mid2);
    OwnRobot[mid2].face(m_world_state.ball.position);
    navigate(mid2,
             m_world_state.ball.position.circleAroundPoint(
                 Common::Angle::fromDeg(-20.0f) +
                     m_world_state.ball.position.angleWith(ownGoal()),
                 650),
             VelocityProfile::aroom());
    OwnRobot[mid2].shoot(0);

    setObstacles(mid1);
    OwnRobot[mid1].face(m_world_state.ball.position);
    navigate(mid1,
             m_world_state.ball.position.circleAroundPoint(
                 Common::Angle::fromDeg(20.0f) +
                     m_world_state.ball.position.angleWith(ownGoal()),
                 650),
             VelocityProfile::aroom());
    OwnRobot[mid1].shoot(0);
#endif

    setObstacles(attack);
    OwnRobot[attack].face(m_world_state.ball.position);
    navigate(attack,
             m_world_state.ball.position.circleAroundPoint(
                 m_world_state.ball.position.angleWith(ownGoal()), 650),
             VelocityProfile::aroom());
    OwnRobot[attack].shoot(0);
}
} // namespace Tyr::Soccer