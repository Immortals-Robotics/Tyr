#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::their_place_ball()
{
    GKHi(gk, true);
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
                    PointOnConnectingLine(ball.Position, Common::Vec2(side * field_width, 0),
                                          DIS(ball.Position, Common::Vec2(side * field_width, 0)) / 3.0f),
                    0, 100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[dmf].Shoot(0);
            }
            else if (own == lmf)
            {
                ERRTSetObstacles(lmf, true, true);
                OwnRobot[lmf].face(Common::Vec2(ball.Position.x, ball.Position.y));
                ERRTNavigate2Point(
                    lmf,
                    CircleAroundPoint(
                        Common::Vec2(ball.Position.x, ball.Position.y),
                        NormalizeAngle(-20 + AngleWith(ball.Position, Common::Vec2(side * field_width, 0))), 650),
                    0, 100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[lmf].Shoot(0);
            }
            else if (own == rmf)
            {
                ERRTSetObstacles(rmf, true, true);
                OwnRobot[rmf].face(Common::Vec2(ball.Position.x, ball.Position.y));
                ERRTNavigate2Point(
                    rmf,
                    CircleAroundPoint(
                        Common::Vec2(ball.Position.x, ball.Position.y),
                        NormalizeAngle(20 + AngleWith(ball.Position, Common::Vec2(side * field_width, 0))), 650),
                    0, 100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[rmf].Shoot(0);
            }
            else if (own == cmf)
            {
                ERRTSetObstacles(cmf, true, true);
                OwnRobot[cmf].face(Common::Vec2(ball.Position.x, ball.Position.y));
                ERRTNavigate2Point(
                    cmf,
                    CircleAroundPoint(Common::Vec2(ball.Position.x, ball.Position.y),
                                      NormalizeAngle(AngleWith(ball.Position, Common::Vec2(side * field_width, 0))),
                                      650),
                    0, 100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[cmf].Shoot(0);
            }
        }
        else
        {
            Mark(own, opp, 500);
        }
    }
#else
    ERRTSetObstacles(dmf, true, true);
    AddCircle(ball.Position.x, ball.Position.y, 1010.0f);
    AddCircle(targetBallPlacement->x, targetBallPlacement->y, 1010.0f);
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OppRobot[i].seenState != Common::CompletelyOut)
        {
            AddCircle(OppRobot[i].Position.x, OppRobot[i].Position.y, 300.0);
        }
    }
    OwnRobot[dmf].face(ball.Position);
    ERRTNavigate2Point(
        dmf,
        ball.Position.pointOnConnectingLine(Common::Vec2(side * field_width, 0),
                                            ball.Position.distanceTo(Common::Vec2(side * field_width, 0)) / 3.0f),
        0, 100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(rw, true, true);
    AddCircle(ball.Position.x, ball.Position.y, 1010.0f);
    AddCircle(targetBallPlacement->x, targetBallPlacement->y, 1010.0f);
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OppRobot[i].seenState != Common::CompletelyOut)
        {
            AddCircle(OppRobot[i].Position.x, OppRobot[i].Position.y, 300.0);
        }
    }
    OwnRobot[rw].face(ball.Position);
    ERRTNavigate2Point(
        rw,
        Common::Vec2(0, -100) +
            ball.Position.pointOnConnectingLine(Common::Vec2(side * field_width, 0),
                                                ball.Position.distanceTo(Common::Vec2(side * field_width, 0)) / 3.0f),
        0, 100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(lw, true, true);
    AddCircle(ball.Position.x, ball.Position.y, 1010.0f);
    AddCircle(targetBallPlacement->x, targetBallPlacement->y, 1010.0f);
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OppRobot[i].seenState != Common::CompletelyOut)
        {
            AddCircle(OppRobot[i].Position.x, OppRobot[i].Position.y, 300.0);
        }
    }
    OwnRobot[lw].face(ball.Position);
    ERRTNavigate2Point(lw,
                       Common::Vec2(0, 100) + ball.Position.pointOnConnectingLine(
                                                  Common::Vec2(side * field_width, 0),
                                                  ball.Position.distanceTo(Common::Vec2(side * field_width, 0)) / 3.0f),
                       0, 100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(lmf, true, true);
    AddCircle(ball.Position.x, ball.Position.y, 1010.0f);
    AddCircle(targetBallPlacement->x, targetBallPlacement->y, 1010.0f);
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OppRobot[i].seenState != Common::CompletelyOut)
        {
            AddCircle(OppRobot[i].Position.x, OppRobot[i].Position.y, 300.0);
        }
    }
    OwnRobot[lmf].face(Common::Vec2(ball.Position.x, ball.Position.y));
    ERRTNavigate2Point(lmf,
                       ball.Position.circleAroundPoint(Common::Angle::fromDeg(-20) +
                                                           ball.Position.angleWith(Common::Vec2(side * field_width, 0)),
                                                       1090),
                       0, 100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(rmf, true, true);
    AddCircle(ball.Position.x, ball.Position.y, 1010.0f);
    AddCircle(targetBallPlacement->x, targetBallPlacement->y, 1010.0f);
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OppRobot[i].seenState != Common::CompletelyOut)
        {
            AddCircle(OppRobot[i].Position.x, OppRobot[i].Position.y, 300.0);
        }
    }
    OwnRobot[rmf].face(Common::Vec2(ball.Position.x, ball.Position.y));
    ERRTNavigate2Point(rmf,
                       ball.Position.circleAroundPoint(Common::Angle::fromDeg(20) +
                                                           ball.Position.angleWith(Common::Vec2(side * field_width, 0)),
                                                       1090),
                       0, 100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(cmf, true, true);
    AddCircle(ball.Position.x, ball.Position.y, 1010.0f);
    AddCircle(targetBallPlacement->x, targetBallPlacement->y, 1010.0f);
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OppRobot[i].seenState != Common::CompletelyOut)
        {
            AddCircle(OppRobot[i].Position.x, OppRobot[i].Position.y, 300.0);
        }
    }
    OwnRobot[cmf].face(ball.Position);
    ERRTNavigate2Point(
        cmf, ball.Position.circleAroundPoint(ball.Position.angleWith(Common::Vec2(side * field_width, 0)), 1090), 0,
        100, &VELOCITY_PROFILE_AROOM);
    // side=-side;
#endif
}
} // namespace Tyr::Soccer
