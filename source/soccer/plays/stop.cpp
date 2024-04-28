#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Stop(void)
{
#if !mark_in_stop
    for (std::map<int *, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i)
    {
        markMap[i->first] = -1;
    }
#endif

    // Common::debug().drawCircle(ball.Position,500,Red);

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
    if (side * OwnRobot[mid1].State.Position.y < side * OwnRobot[mid2].State.Position.y) // Nice :D
    {
        std::swap(mid1, mid2);
    }

    GKHi(gk, true);
    // DefHi(def,NULL, true);
    DefMid(def, rw, lw, NULL, true);

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
                OwnRobot[dmf].face(ball.Position);
                ERRTNavigate2Point(
                    dmf,
                    pointOnConnectingLine(ball.Position, Common::Vec2(side * field_width, 0),
                                          Common::Vec2::distance(ball.Position, Common::Vec2(side * field_width, 0)) /
                                              3.0f),
                    0, 100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[dmf].Shoot(0);
            }
            else if (own == lmf)
            {
                ERRTSetObstacles(lmf, true, true);
                OwnRobot[lmf].face(ball.Position);
                ERRTNavigate2Point(
                    lmf,
                    circleAroundPoint(ball.Position,
                                      NormalizeAngle(-20 + Common::Vec2::angleWith(
                                                               ball.Position, Common::Vec2(side * field_width, 0))),
                                      650),
                    0, 100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[lmf].Shoot(0);
            }
            else if (own == rmf)
            {
                ERRTSetObstacles(rmf, true, true);
                OwnRobot[rmf].face(ball.Position);
                ERRTNavigate2Point(
                    rmf,
                    circleAroundPoint(ball.Position,
                                      NormalizeAngle(20 + Common::Vec2::angleWith(ball.Position,
                                                                                  Common::Vec2(side * field_width, 0))),
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
    OwnRobot[dmf].face(ball.Position);
    ERRTNavigate2Point(
        dmf,
        ball.Position.pointOnConnectingLine(Common::Vec2(side * field_width, 0),
                                            ball.Position.distanceTo(Common::Vec2(side * field_width, 0)) / 3.0f),
        0, 100, &VELOCITY_PROFILE_AROOM);
    OwnRobot[dmf].Shoot(0);

    ERRTSetObstacles(lmf, true, true);
    OwnRobot[lmf].face(ball.Position);
    ERRTNavigate2Point(lmf,
                       ball.Position.circleAroundPoint(Common::Angle::fromDeg(-20.0f) +
                                                           ball.Position.angleWith(Common::Vec2(side * field_width, 0)),
                                                       650),
                       0, 100, &VELOCITY_PROFILE_AROOM);
    OwnRobot[lmf].Shoot(0);

    ERRTSetObstacles(rmf, true, true);
    OwnRobot[rmf].face(ball.Position);
    ERRTNavigate2Point(rmf,
                       ball.Position.circleAroundPoint(Common::Angle::fromDeg(20.0f) +
                                                           ball.Position.angleWith(Common::Vec2(side * field_width, 0)),
                                                       650),
                       0, 100, &VELOCITY_PROFILE_AROOM);
    OwnRobot[rmf].Shoot(0);
#endif

    ERRTSetObstacles(cmf, true, true);
    OwnRobot[cmf].face(ball.Position);
    ERRTNavigate2Point(
        cmf, ball.Position.circleAroundPoint(ball.Position.angleWith(Common::Vec2(side * field_width, 0)), 650), 0, 100,
        &VELOCITY_PROFILE_AROOM);
    OwnRobot[cmf].Shoot(0);
}
} // namespace Tyr::Soccer