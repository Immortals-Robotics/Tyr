#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::their_place_ball()
{
    GKHi(gk, true);
    DefBy3(def, rw, lw, nullptr, true);

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
                OwnRobot[dmf].face(m_world_state.ball.position);
                ERRTNavigate2Point(
                    dmf,
                    PointOnConnectingLine(
                        m_world_state.ball.position, Common::Vec2(side * Common::field().width, 0),
                        DIS(m_world_state.ball.position, Common::Vec2(side * Common::field().width, 0)) / 3.0f),
                    100, VelocityProfile::Type::Aroom);
                OwnRobot[dmf].Shoot(0);
            }
            else if (own == lmf)
            {
                ERRTSetObstacles(lmf, true, true);
                OwnRobot[lmf].face(m_world_state.ball.position);
                ERRTNavigate2Point(
                    lmf,
                    CircleAroundPoint(m_world_state.ball.position,
                                      NormalizeAngle(-20 + AngleWith(m_world_state.ball.position,
                                                                     Common::Vec2(side * Common::field().width, 0))),
                                      650),
                    100, VelocityProfile::Type::Aroom);
                OwnRobot[lmf].Shoot(0);
            }
            else if (own == rmf)
            {
                ERRTSetObstacles(rmf, true, true);
                OwnRobot[rmf].face(m_world_state.ball.position);
                ERRTNavigate2Point(
                    rmf,
                    CircleAroundPoint(m_world_state.ball.position,
                                      NormalizeAngle(20 + AngleWith(m_world_state.ball.position,
                                                                    Common::Vec2(side * Common::field().width, 0))),
                                      650),
                    100, VelocityProfile::Type::Aroom);
                OwnRobot[rmf].Shoot(0);
            }
            else if (own == cmf)
            {
                ERRTSetObstacles(cmf, true, true);
                OwnRobot[cmf].face(m_world_state.ball.position);
                ERRTNavigate2Point(
                    cmf,
                    CircleAroundPoint(m_world_state.ball.position,
                                      NormalizeAngle(AngleWith(m_world_state.ball.position,
                                                               Common::Vec2(side * Common::field().width, 0))),
                                      650),
                    100, VelocityProfile::Type::Aroom);
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
    ERRTNavigate2Point(
        dmf,
        m_world_state.ball.position.pointOnConnectingLine(
            Common::Vec2(side * Common::field().width, 0),
            m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(rw, true, true);
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
    ERRTNavigate2Point(
        rw,
        Common::Vec2(0, -100) +
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(side * Common::field().width, 0),
                m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(lw, true, true);
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
    ERRTNavigate2Point(
        lw,
        Common::Vec2(0, 100) +
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(side * Common::field().width, 0),
                m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(lmf, true, true);
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
    ERRTNavigate2Point(lmf,
                       m_world_state.ball.position.circleAroundPoint(
                           Common::Angle::fromDeg(-20) +
                               m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)),
                           1090),
                       100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(rmf, true, true);
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
    ERRTNavigate2Point(rmf,
                       m_world_state.ball.position.circleAroundPoint(
                           Common::Angle::fromDeg(20) +
                               m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)),
                           1090),
                       100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(cmf, true, true);
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
    ERRTNavigate2Point(cmf,
                       m_world_state.ball.position.circleAroundPoint(
                           m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)), 1090),
                       100, VelocityProfile::Type::Aroom);
    // side=-side;
#endif
}
} // namespace Tyr::Soccer
