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
                             m_world_state.ball.position, ownGoal(),
                             DIS(m_world_state.ball.position, ownGoal()) / 3.0f),
                         VelocityProfile::aroom());
                OwnRobot[dmf].shoot(0);
            }
            else if (own == mid2)
            {
                setObstacles(mid2);
                OwnRobot[mid2].face(m_world_state.ball.position);
                navigate(
                    mid2,
                    CircleAroundPoint(m_world_state.ball.position,
                                      NormalizeAngle(-20 + AngleWith(m_world_state.ball.position,
                                                                     ownGoal())),
                                      650),
                    VelocityProfile::aroom());
                OwnRobot[mid2].shoot(0);
            }
            else if (own == mid1)
            {
                setObstacles(mid1);
                OwnRobot[mid1].face(m_world_state.ball.position);
                navigate(
                    mid1,
                    CircleAroundPoint(m_world_state.ball.position,
                                      NormalizeAngle(20 + AngleWith(m_world_state.ball.position,
                                                                    ownGoal())),
                                      650),
                    VelocityProfile::aroom());
                OwnRobot[mid1].shoot(0);
            }
            else if (own == attack)
            {
                setObstacles(attack);
                OwnRobot[attack].face(m_world_state.ball.position);
                navigate(attack,
                         CircleAroundPoint(m_world_state.ball.position,
                                           NormalizeAngle(AngleWith(m_world_state.ball.position,
                                                                    ownGoal())),
                                           650),
                         VelocityProfile::aroom());
                OwnRobot[attack].shoot(0);
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
                 ownGoal(),
                 m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
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
                     ownGoal(),
                     m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
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
                     ownGoal(),
                     m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
             VelocityProfile::aroom());

    setObstacles(mid2);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    g_obs_map.addCircle({m_ref_state.place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            g_obs_map.addCircle({m_world_state.opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[mid2].face(m_world_state.ball.position);
    navigate(mid2,
             m_world_state.ball.position.circleAroundPoint(
                 Common::Angle::fromDeg(-20) +
                     m_world_state.ball.position.angleWith(ownGoal()),
                 1090),
             VelocityProfile::aroom());

    setObstacles(mid1);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    g_obs_map.addCircle({m_ref_state.place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            g_obs_map.addCircle({m_world_state.opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[mid1].face(m_world_state.ball.position);
    navigate(mid1,
             m_world_state.ball.position.circleAroundPoint(
                 Common::Angle::fromDeg(20) +
                     m_world_state.ball.position.angleWith(ownGoal()),
                 1090),
             VelocityProfile::aroom());

    setObstacles(attack);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    g_obs_map.addCircle({m_ref_state.place_ball_target, 1010.0f});
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            g_obs_map.addCircle({m_world_state.opp_robot[i].position, 300.0f});
        }
    }
    OwnRobot[attack].face(m_world_state.ball.position);
    navigate(attack,
             m_world_state.ball.position.circleAroundPoint(
                 m_world_state.ball.position.angleWith(ownGoal()), 1090),
             VelocityProfile::aroom());
    // side=-side;
#endif
}
} // namespace Tyr::Soccer
