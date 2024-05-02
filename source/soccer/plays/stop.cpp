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

    // Common::debug().drawCircle(ball.position,500,Red);

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
    if (side * OwnRobot[mid1].State.position.y < side * OwnRobot[mid2].State.position.y) // Nice :D
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
                OwnRobot[dmf].face(ball.position);
                ERRTNavigate2Point(
                    dmf,
                    pointOnConnectingLine(ball.position, Common::Vec2(side * Common::worldState().field.width, 0),
                                          Common::Vec2::distance(ball.position, Common::Vec2(side * Common::worldState().field.width, 0)) /
                                              3.0f),
                    0, 100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[dmf].Shoot(0);
            }
            else if (own == lmf)
            {
                ERRTSetObstacles(lmf, true, true);
                OwnRobot[lmf].face(ball.position);
                ERRTNavigate2Point(
                    lmf,
                    circleAroundPoint(ball.position,
                                      NormalizeAngle(-20 + Common::Vec2::angleWith(
                                                               ball.position, Common::Vec2(side * Common::worldState().field.width, 0))),
                                      650),
                    0, 100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[lmf].Shoot(0);
            }
            else if (own == rmf)
            {
                ERRTSetObstacles(rmf, true, true);
                OwnRobot[rmf].face(ball.position);
                ERRTNavigate2Point(
                    rmf,
                    circleAroundPoint(ball.position,
                                      NormalizeAngle(20 + Common::Vec2::angleWith(ball.position,
                                                                                  Common::Vec2(side * Common::worldState().field.width, 0))),
                                      650),
                    0, 100, &VELOCITY_PROFILE_AROOM);
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
    OwnRobot[dmf].face(ball.position);
    ERRTNavigate2Point(
        dmf,
        ball.position.pointOnConnectingLine(Common::Vec2(side * Common::worldState().field.width, 0),
                                            ball.position.distanceTo(Common::Vec2(side * Common::worldState().field.width, 0)) / 3.0f),
        0, 100, &VELOCITY_PROFILE_AROOM);
    OwnRobot[dmf].Shoot(0);

    ERRTSetObstacles(lmf, true, true);
    OwnRobot[lmf].face(ball.position);
    ERRTNavigate2Point(lmf,
                       ball.position.circleAroundPoint(Common::Angle::fromDeg(-20.0f) +
                                                           ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)),
                                                       650),
                       0, 100, &VELOCITY_PROFILE_AROOM);
    OwnRobot[lmf].Shoot(0);

    ERRTSetObstacles(rmf, true, true);
    OwnRobot[rmf].face(ball.position);
    ERRTNavigate2Point(rmf,
                       ball.position.circleAroundPoint(Common::Angle::fromDeg(20.0f) +
                                                           ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)),
                                                       650),
                       0, 100, &VELOCITY_PROFILE_AROOM);
    OwnRobot[rmf].Shoot(0);
#endif

    ERRTSetObstacles(cmf, true, true);
    OwnRobot[cmf].face(ball.position);
    ERRTNavigate2Point(
        cmf, ball.position.circleAroundPoint(ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)), 650), 0, 100,
        &VELOCITY_PROFILE_AROOM);
    OwnRobot[cmf].Shoot(0);
}
} // namespace Tyr::Soccer