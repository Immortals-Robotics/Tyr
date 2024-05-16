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

    // Common::debug().draw({m_world_state.ball.position,500},Red);

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
                navigate(
                    dmf,
                    pointOnConnectingLine(m_world_state.ball.position, Common::Vec2(side * Common::field().width, 0),
                                          Common::Vec2::distance(m_world_state.ball.position,
                                                                 Common::Vec2(side * Common::field().width, 0)) /
                                              3.0f),
                    100, VelocityProfile::Type::Aroom);
                OwnRobot[dmf].shoot(0);
            }
            else if (own == lmf)
            {
                setObstacles(lmf);
                OwnRobot[lmf].face(m_world_state.ball.position);
                navigate(
                    lmf,
                    circleAroundPoint(
                        m_world_state.ball.position,
                        NormalizeAngle(-20 + Common::Vec2::angleWith(m_world_state.ball.position,
                                                                     Common::Vec2(side * Common::field().width, 0))),
                        650),
                    100, VelocityProfile::Type::Aroom);
                OwnRobot[lmf].shoot(0);
            }
            else if (own == rmf)
            {
                setObstacles(rmf);
                OwnRobot[rmf].face(m_world_state.ball.position);
                navigate(
                    rmf,
                    circleAroundPoint(
                        m_world_state.ball.position,
                        NormalizeAngle(20 + Common::Vec2::angleWith(m_world_state.ball.position,
                                                                    Common::Vec2(side * Common::field().width, 0))),
                        650),
                    100, VelocityProfile::Type::Aroom);
                OwnRobot[rmf].shoot(0);
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
    navigate(
        dmf,
        m_world_state.ball.position.pointOnConnectingLine(
            Common::Vec2(side * Common::field().width, 0),
            m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);
    OwnRobot[dmf].shoot(0);

    setObstacles(lmf);
    OwnRobot[lmf].face(m_world_state.ball.position);
    navigate(lmf,
                       m_world_state.ball.position.circleAroundPoint(
                           Common::Angle::fromDeg(-20.0f) +
                               m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)),
                           650),
                       100, VelocityProfile::Type::Aroom);
    OwnRobot[lmf].shoot(0);

    setObstacles(rmf);
    OwnRobot[rmf].face(m_world_state.ball.position);
    navigate(rmf,
                       m_world_state.ball.position.circleAroundPoint(
                           Common::Angle::fromDeg(20.0f) +
                               m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)),
                           650),
                       100, VelocityProfile::Type::Aroom);
    OwnRobot[rmf].shoot(0);
#endif

    setObstacles(cmf);
    OwnRobot[cmf].face(m_world_state.ball.position);
    navigate(cmf,
                       m_world_state.ball.position.circleAroundPoint(
                           m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)), 650),
                       100, VelocityProfile::Type::Aroom);
    OwnRobot[cmf].shoot(0);
}
} // namespace Tyr::Soccer