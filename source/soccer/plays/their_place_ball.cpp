#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::their_place_ball()
{
    GKHi(gk, true);
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
                    PointOnConnectingLine(Common::worldState().ball.position, Common::Vec2(side * Common::worldState().field.width, 0),
                                          DIS(Common::worldState().ball.position, Common::Vec2(side * Common::worldState().field.width, 0)) / 3.0f),
                    0, 100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[dmf].Shoot(0);
            }
            else if (own == lmf)
            {
                ERRTSetObstacles(lmf, true, true);
                OwnRobot[lmf].face(Common::worldState().ball.position);
                ERRTNavigate2Point(
                    lmf,
                    CircleAroundPoint(
                        Common::worldState().ball.position,
                        NormalizeAngle(-20 + AngleWith(Common::worldState().ball.position, Common::Vec2(side * Common::worldState().field.width, 0))), 650),
                    0, 100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[lmf].Shoot(0);
            }
            else if (own == rmf)
            {
                ERRTSetObstacles(rmf, true, true);
                OwnRobot[rmf].face(Common::worldState().ball.position);
                ERRTNavigate2Point(
                    rmf,
                    CircleAroundPoint(
                        Common::worldState().ball.position,
                        NormalizeAngle(20 + AngleWith(Common::worldState().ball.position, Common::Vec2(side * Common::worldState().field.width, 0))), 650),
                    0, 100, &VELOCITY_PROFILE_AROOM);
                OwnRobot[rmf].Shoot(0);
            }
            else if (own == cmf)
            {
                ERRTSetObstacles(cmf, true, true);
                OwnRobot[cmf].face(Common::worldState().ball.position);
                ERRTNavigate2Point(
                    cmf,
                    CircleAroundPoint(Common::worldState().ball.position,
                                      NormalizeAngle(AngleWith(Common::worldState().ball.position, Common::Vec2(side * Common::worldState().field.width, 0))),
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
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    obs_map.addCircle({Common::refereeState().place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (Common::worldState().opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            obs_map.addCircle({Common::worldState().opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[dmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        dmf,
        Common::worldState().ball.position.pointOnConnectingLine(Common::Vec2(side * Common::worldState().field.width, 0),
                                            Common::worldState().ball.position.distanceTo(Common::Vec2(side * Common::worldState().field.width, 0)) / 3.0f),
        0, 100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(rw, true, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    obs_map.addCircle({Common::refereeState().place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (Common::worldState().opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            obs_map.addCircle({Common::worldState().opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[rw].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        rw,
        Common::Vec2(0, -100) +
            Common::worldState().ball.position.pointOnConnectingLine(Common::Vec2(side * Common::worldState().field.width, 0),
                                                Common::worldState().ball.position.distanceTo(Common::Vec2(side * Common::worldState().field.width, 0)) / 3.0f),
        0, 100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(lw, true, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    obs_map.addCircle({Common::refereeState().place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (Common::worldState().opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            obs_map.addCircle({Common::worldState().opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[lw].face(Common::worldState().ball.position);
    ERRTNavigate2Point(lw,
                       Common::Vec2(0, 100) + Common::worldState().ball.position.pointOnConnectingLine(
                                                  Common::Vec2(side * Common::worldState().field.width, 0),
                                                  Common::worldState().ball.position.distanceTo(Common::Vec2(side * Common::worldState().field.width, 0)) / 3.0f),
                       0, 100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(lmf, true, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    obs_map.addCircle({Common::refereeState().place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (Common::worldState().opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            obs_map.addCircle({Common::worldState().opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[lmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(lmf,
                       Common::worldState().ball.position.circleAroundPoint(Common::Angle::fromDeg(-20) +
                                                           Common::worldState().ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)),
                                                       1090),
                       0, 100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(rmf, true, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    obs_map.addCircle({Common::refereeState().place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (Common::worldState().opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            obs_map.addCircle({Common::worldState().opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[rmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(rmf,
                       Common::worldState().ball.position.circleAroundPoint(Common::Angle::fromDeg(20) +
                                                           Common::worldState().ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)),
                                                       1090),
                       0, 100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(cmf, true, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    obs_map.addCircle({Common::refereeState().place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (Common::worldState().opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            obs_map.addCircle({Common::worldState().opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[cmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        cmf, Common::worldState().ball.position.circleAroundPoint(Common::worldState().ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)), 1090), 0,
        100, &VELOCITY_PROFILE_AROOM);
    // side=-side;
#endif
}
} // namespace Tyr::Soccer
