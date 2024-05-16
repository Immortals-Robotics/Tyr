#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::their_place_ball()
{
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
                         PointOnConnectingLine(
                             m_world_state.ball.position, Common::Vec2(side * Common::field().width, 0),
                             DIS(m_world_state.ball.position, Common::Vec2(side * Common::field().width, 0)) / 3.0f),
                         VelocityProfile::aroom());
                OwnRobot[dmf].shoot(0);
            }
            else if (own == lmf)
            {
                setObstacles(lmf);
                OwnRobot[lmf].face(m_world_state.ball.position);
                navigate(
                    lmf,
                    CircleAroundPoint(m_world_state.ball.position,
                                      NormalizeAngle(-20 + AngleWith(m_world_state.ball.position,
                                                                     Common::Vec2(side * Common::field().width, 0))),
                                      650),
                    VelocityProfile::aroom());
                OwnRobot[lmf].shoot(0);
            }
            else if (own == rmf)
            {
                setObstacles(rmf);
                OwnRobot[rmf].face(m_world_state.ball.position);
                navigate(
                    rmf,
                    CircleAroundPoint(m_world_state.ball.position,
                                      NormalizeAngle(20 + AngleWith(m_world_state.ball.position,
                                                                    Common::Vec2(side * Common::field().width, 0))),
                                      650),
                    VelocityProfile::aroom());
                OwnRobot[rmf].shoot(0);
            }
            else if (own == cmf)
            {
                setObstacles(cmf);
                OwnRobot[cmf].face(m_world_state.ball.position);
                navigate(cmf,
                         CircleAroundPoint(m_world_state.ball.position,
                                           NormalizeAngle(AngleWith(m_world_state.ball.position,
                                                                    Common::Vec2(side * Common::field().width, 0))),
                                           650),
                         VelocityProfile::aroom());
                OwnRobot[cmf].shoot(0);
            }
        }
        else
        {
            Mark(own, opp, 500);
        }
    }
#else
    setObstacles(dmf);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    g_obs_map.addCircle({m_ref_state.place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            g_obs_map.addCircle({m_world_state.opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[dmf].face(m_world_state.ball.position);
    navigate(dmf,
             m_world_state.ball.position.pointOnConnectingLine(
                 Common::Vec2(side * Common::field().width, 0),
                 m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
             VelocityProfile::aroom());

    setObstacles(rw);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    g_obs_map.addCircle({m_ref_state.place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            g_obs_map.addCircle({m_world_state.opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[rw].face(m_world_state.ball.position);
    navigate(rw,
             Common::Vec2(0, -100) +
                 m_world_state.ball.position.pointOnConnectingLine(
                     Common::Vec2(side * Common::field().width, 0),
                     m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
             VelocityProfile::aroom());

    setObstacles(lw);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    g_obs_map.addCircle({m_ref_state.place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            g_obs_map.addCircle({m_world_state.opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[lw].face(m_world_state.ball.position);
    navigate(lw,
             Common::Vec2(0, 100) +
                 m_world_state.ball.position.pointOnConnectingLine(
                     Common::Vec2(side * Common::field().width, 0),
                     m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
             VelocityProfile::aroom());

    setObstacles(lmf);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    g_obs_map.addCircle({m_ref_state.place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            g_obs_map.addCircle({m_world_state.opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[lmf].face(m_world_state.ball.position);
    navigate(lmf,
             m_world_state.ball.position.circleAroundPoint(
                 Common::Angle::fromDeg(-20) +
                     m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)),
                 1090),
             VelocityProfile::aroom());

    setObstacles(rmf);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    g_obs_map.addCircle({m_ref_state.place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            g_obs_map.addCircle({m_world_state.opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[rmf].face(m_world_state.ball.position);
    navigate(rmf,
             m_world_state.ball.position.circleAroundPoint(
                 Common::Angle::fromDeg(20) +
                     m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)),
                 1090),
             VelocityProfile::aroom());

    setObstacles(cmf);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    g_obs_map.addCircle({m_ref_state.place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            g_obs_map.addCircle({m_world_state.opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[cmf].face(m_world_state.ball.position);
    navigate(cmf,
             m_world_state.ball.position.circleAroundPoint(
                 m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)), 1090),
             VelocityProfile::aroom());
    // side=-side;
#endif
}
} // namespace Tyr::Soccer
