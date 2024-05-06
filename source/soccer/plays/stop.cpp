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

    // Common::debug().draw({Common::worldState().ball.position,500},Red);

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
    // DefHi(def,nullptr, true);
    DefMid(def, rw, lw, nullptr, true);

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
                ERRTSetObstacles(dmf, true, true);
                OwnRobot[dmf].face(Common::worldState().ball.position);
                ERRTNavigate2Point(
                    dmf,
                    pointOnConnectingLine(
                        Common::worldState().ball.position, Common::Vec2(side * Common::worldState().field.width, 0),
                        Common::Vec2::distance(Common::worldState().ball.position,
                                               Common::Vec2(side * Common::worldState().field.width, 0)) /
                            3.0f),
                    100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[dmf].Shoot(0);
            }
            else if (own == lmf)
            {
                ERRTSetObstacles(lmf, true, true);
                OwnRobot[lmf].face(Common::worldState().ball.position);
                ERRTNavigate2Point(
                    lmf,
                    circleAroundPoint(
                        Common::worldState().ball.position,
                        NormalizeAngle(
                            -20 + Common::Vec2::angleWith(Common::worldState().ball.position,
                                                          Common::Vec2(side * Common::worldState().field.width, 0))),
                        650),
                    100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[lmf].Shoot(0);
            }
            else if (own == rmf)
            {
                ERRTSetObstacles(rmf, true, true);
                OwnRobot[rmf].face(Common::worldState().ball.position);
                ERRTNavigate2Point(
                    rmf,
                    circleAroundPoint(
                        Common::worldState().ball.position,
                        NormalizeAngle(
                            20 + Common::Vec2::angleWith(Common::worldState().ball.position,
                                                         Common::Vec2(side * Common::worldState().field.width, 0))),
                        650),
                    100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[rmf].Shoot(0);
            }
        }
        else
        {
            Mark(own, opp, 500);
        }
    }
#else
    ERRTSetObstacles(dmf, true, true);
    OwnRobot[dmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        dmf,
        Common::worldState().ball.position.pointOnConnectingLine(
            Common::Vec2(side * Common::worldState().field.width, 0),
            Common::worldState().ball.position.distanceTo(Common::Vec2(side * Common::worldState().field.width, 0)) /
                3.0f),
        100, &VELOCITY_PROFILE_AROOM);
    OwnRobot[dmf].Shoot(0);

    ERRTSetObstacles(lmf, true, true);
    OwnRobot[lmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(lmf,
                       Common::worldState().ball.position.circleAroundPoint(
                           Common::Angle::fromDeg(-20.0f) + Common::worldState().ball.position.angleWith(Common::Vec2(
                                                                side * Common::worldState().field.width, 0)),
                           650),
                       100, &VELOCITY_PROFILE_AROOM);
    OwnRobot[lmf].Shoot(0);

    ERRTSetObstacles(rmf, true, true);
    OwnRobot[rmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(rmf,
                       Common::worldState().ball.position.circleAroundPoint(
                           Common::Angle::fromDeg(20.0f) + Common::worldState().ball.position.angleWith(Common::Vec2(
                                                               side * Common::worldState().field.width, 0)),
                           650),
                       100, &VELOCITY_PROFILE_AROOM);
    OwnRobot[rmf].Shoot(0);
#endif

    ERRTSetObstacles(cmf, true, true);
    OwnRobot[cmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        cmf,
        Common::worldState().ball.position.circleAroundPoint(
            Common::worldState().ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)),
            650),
        100, &VELOCITY_PROFILE_AROOM);
    OwnRobot[cmf].Shoot(0);
}
} // namespace Tyr::Soccer