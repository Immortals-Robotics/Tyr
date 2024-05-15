

#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::our_place_ball_shoot()
{

    GKHi(gk, true);
    DefBy3(def, rw, lw, nullptr, true);

    static Common::Angle move_angle, temp_opp_ang;
    if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) > 100)
    {
        move_angle   = m_ref_state.place_ball_target.angleWith(m_world_state.ball.position);
        temp_opp_ang = Common::Angle::fromDeg(move_angle.deg() + 180.0f);
    }

    static Common::Angle t_ang, t_opp_ang;
    static Common::Vec2  t_pos;
    static Common::Timer temp_time;

    Common::logDebug("BEFORE: {}_{}", dmf, mid2);
    //    wantThisRobot(attack);
    //    wantThisRobot(dmf);
    positionRobots();
    Common::logDebug("AFTER: {}_{}", dmf, mid2);

    /*ERRTSetObstacles ( mid1 , false , true );
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[mid1].face(m_world_state.ball.position);
    ERRTNavigate2Point ( mid1 , PointOnConnectingLine(m_world_state.ball.position,
    Common::Vec2(side*Common::field().width, 0), DIS(m_world_state.ball.position,
    Common::Vec2(side*Common::field().width, 0))/3.0f) ,0 , 100,VelocityProfile::Type::Aroom);*/

    ERRTSetObstacles(rw, false, true);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[rw].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        rw,
        Common::Vec2(0, -100) +
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(side * Common::field().width, 0),
                m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(lw, false, true);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[lw].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        lw,
        Common::Vec2(0, 100) +
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(side * Common::field().width, 0),
                m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(mid2, false, true);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[mid2].face(m_world_state.ball.position);
    ERRTNavigate2Point(mid2,
                       m_world_state.ball.position.circleAroundPoint(
                           m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)), 1090),
                       100, VelocityProfile::Type::Aroom);

    OwnRobot[attack].target.velocity.x = 0.0;
    OwnRobot[attack].target.velocity.y = 0.0;
    OwnRobot[dmf].target.velocity.x    = 0.0;
    OwnRobot[dmf].target.velocity.y    = 0.0;
    //    g_obs_map.resetMap();
    /*if(timer.time()>7 && FUNC_state < 2){//What the hell are you waiting for?!?!
        FUNC_state = 3;
        FUNC_CNT = 0;
    }*/
    if (FUNC_state == 0)
    {
        if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) < 95)
        {
            FUNC_state = 7;
            FUNC_CNT   = 0;
            t_ang.setDeg(0.0f);
            t_opp_ang.setDeg(180.0f);
        }
        else if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) < 1000)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        //        OwnRobot[attack].target.angle = temp_opp_ang;
        OwnRobot[dmf].target.angle = move_angle;
        //        ERRTSetObstacles(attack,0,0);
        //        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});
        //        ERRTNavigate2Point(attack, CircleAroundPoint(m_world_state.ball.position, move_angle, 250), 0,
        //        40, VelocityProfile::Type::Aroom);

        ERRTSetObstacles(dmf, 0, 0);
        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});
        ERRTNavigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(temp_opp_ang, 250), 40,
                           VelocityProfile::Type::Aroom);

        g_obs_map.resetMap();
        circle_ball_free(attack, move_angle, 0, 0, 0.0);
        Common::logDebug(":::{}", OwnRobot[attack].state().velocity.length());
        Common::logDebug("{}", OwnRobot[dmf].state().velocity.length());

        if (OwnRobot[attack].state().velocity.length() < 20 && OwnRobot[dmf].state().velocity.length() < 20)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 1;
            FUNC_CNT   = 0;
        }
    }
    else if (FUNC_state == 1)
    {

        circle_ball_free(attack, move_angle, 18, 0, 0.0);
        WaitForPass(dmf, false, &OwnRobot[attack].state().position);
        if (OwnRobot[attack].state().position.distanceTo(m_world_state.ball.position) > 400)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 20)
        {
            FUNC_state = 2;
            FUNC_CNT   = 0;
            temp_time.start();
        }
    }
    else if (FUNC_state == 2)
    {

        WaitForPass(dmf, false, &OwnRobot[attack].state().position);
        //        OwnRobot[dmf].target.angle = 90;

        if (temp_time.time() > 2)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
            temp_time.start();
        }
    }
    else if (FUNC_state == 3)
    {

        if (std::fabs((OwnRobot[attack].target.angle - move_angle).deg()) > 90)
            std::swap(move_angle, temp_opp_ang);
        OwnRobot[attack].target.angle = move_angle;
        OwnRobot[dmf].target.angle    = temp_opp_ang;
        ERRTSetObstacles(attack, 0, 0);
        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});
        ERRTNavigate2Point(attack, m_world_state.ball.position.circleAroundPoint(temp_opp_ang, 250), 40,
                           VelocityProfile::Type::Aroom);
        ERRTSetObstacles(dmf, 0, 0);
        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});

        ERRTNavigate2Point(dmf, m_world_state.ball.position.circleAroundPoint(move_angle, 250), 40,
                           VelocityProfile::Type::Aroom);

        if (m_world_state.ball.position.circleAroundPoint(temp_opp_ang, 250)
                    .distanceTo(OwnRobot[attack].state().position) < 100 &&
            m_world_state.ball.position.circleAroundPoint(move_angle, 250).distanceTo(OwnRobot[dmf].state().position) <
                100 &&
            m_world_state.ball.velocity.length() < 10)
        {

            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 4;
            FUNC_CNT   = 0;
            t_pos      = m_world_state.ball.position;
        }
    }
    else if (FUNC_state == 4)
    {
        if (std::fabs((OwnRobot[attack].target.angle - move_angle).deg()) > 90)
            std::swap(move_angle, temp_opp_ang);

        //        if(AngleWith(OwnRobot[attack].state().position,OwnRobot[dmf].state().position))

        OwnRobot[attack].target.angle = move_angle;
        OwnRobot[dmf].target.angle    = temp_opp_ang;
        Navigate2Point(attack, t_pos.circleAroundPoint(temp_opp_ang, 75), 20,
                       VelocityProfile::Type::BallPlaceKheyliSooski);

        Navigate2Point(dmf, t_pos.circleAroundPoint(move_angle, 75), 20, VelocityProfile::Type::BallPlaceKheyliSooski);

        bool ball_has_slipt = ((OwnRobot[attack].state().position + OwnRobot[dmf].state().position) / 2)
                                  .distanceTo(m_world_state.ball.position) > 300;

        if (m_world_state.ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        if (t_pos.circleAroundPoint(temp_opp_ang, 75).distanceTo(OwnRobot[attack].state().position) < 40 &&
            t_pos.circleAroundPoint(move_angle, 75).distanceTo(OwnRobot[dmf].state().position) < 40)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 5;
            FUNC_CNT   = 0;
            t_ang      = move_angle;
            t_opp_ang  = temp_opp_ang;
        }
    }
    else if (FUNC_state == 5)
    {
        OwnRobot[attack].target.angle = t_ang;
        OwnRobot[dmf].target.angle    = t_opp_ang;
        Navigate2Point(attack, m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 75), 20,
                       VelocityProfile::Type::BallPlaceKheyliSooski);

        Navigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(t_ang, 75), 20,
                       VelocityProfile::Type::BallPlaceKheyliSooski);

        bool ball_has_slipt = ((OwnRobot[attack].state().position + OwnRobot[dmf].state().position) / 2)
                                  .distanceTo(m_world_state.ball.position) > 300;

        if (m_world_state.ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        if (m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 75)
                    .distanceTo(OwnRobot[attack].state().position) < 40 &&
            m_ref_state.place_ball_target.circleAroundPoint(t_ang, 75).distanceTo(OwnRobot[dmf].state().position) < 40)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 30)
        {
            FUNC_state = 6;
            FUNC_CNT   = 0;
        }
    }
    else if (FUNC_state == 6)
    {

        OwnRobot[attack].target.angle = t_ang;
        OwnRobot[dmf].target.angle    = t_opp_ang;
        // OwnRobot[attack].face(m_world_state.ball.position);
        Navigate2Point(attack, m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 250), 20,
                       VelocityProfile::Type::BallPlaceKheyliSooski);
        // OwnRobot[dmf].face(m_world_state.ball.position);
        Navigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(t_ang, 250), 20,
                       VelocityProfile::Type::BallPlaceKheyliSooski);

        if (m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 250)
                    .distanceTo(OwnRobot[attack].state().position) < 40 &&
            m_ref_state.place_ball_target.circleAroundPoint(t_ang, 250).distanceTo(OwnRobot[dmf].state().position) < 40)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 30)
        {
            if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) > 95)
            {
                FUNC_state = 3;
                FUNC_CNT   = 0;
            }
            else
            {
                FUNC_state = 7;
                FUNC_CNT   = 0;
            }
        }
    }
    else
    {
        //        OwnRobot[attack].face(m_world_state.ball.position);
        //        Navigate2Point(attack,OwnRobot[attack].state().position,false,0,VelocityProfile::Type::BallPlaceKheyliSooski);
        OwnRobot[attack].face(m_world_state.ball.position); // TODO test this
        ERRTSetObstacles(attack, 1, 1);
        ERRTNavigate2Point(attack, m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 550), 20,
                           VelocityProfile::Type::BallPlaceKheyliSooski);
        OwnRobot[dmf].face(m_world_state.ball.position);
        ERRTSetObstacles(dmf, 1, 1);
        ERRTNavigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(t_ang, 550), 20,
                           VelocityProfile::Type::BallPlaceKheyliSooski);

        //        OwnRobot[attack].target.angle = 0;
        //        OwnRobot[dmf].target.angle = 0;
        //        t_ang = NormalizeAngle(OwnRobot[attack].state().angle + 180);
        //        Navigate2Point(attack, CircleAroundPoint(m_world_state.ball.position, t_ang, 170), 0, 20,
        //        VelocityProfile::Type::BallPlaceKheyliSooski); t_ang = NormalizeAngle(OwnRobot[dmf].state().angle +
        //        180); Navigate2Point(dmf, CircleAroundPoint(m_world_state.ball.position, t_ang, 170),  20,
        //        VelocityProfile::Type::BallPlaceKheyliSooski);

        bool success = m_ref_state.place_ball_target.distanceTo(m_world_state.ball.position) < 100.0;

        Common::logDebug("______IN___STATE_DONE_____");
        Common::logDebug("{}", m_ref_state.place_ball_target - m_world_state.ball.position);
        if (success)
        {
            Common::logDebug("MADE it!!!");
        }
        else
        {
            Common::logWarning("lost it!!!");
        }
    }
    Common::logDebug("______IN___STATE_{}_____", FUNC_state);

    Common::logDebug("___DIS___{}", m_ref_state.place_ball_target.distanceTo(m_world_state.ball.position));
}

void Ai::our_place_ball_shoot_V2()
{

    GKHi(gk, true);
    DefBy3(def, rw, lw, nullptr, true);

    static Common::Angle move_angle, temp_opp_ang;
    if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) > 100)
    {
        move_angle   = m_ref_state.place_ball_target.angleWith(m_world_state.ball.position);
        temp_opp_ang = move_angle + Common::Angle::fromDeg(180.0f);
    }

    static Common::Angle t_ang, t_opp_ang;
    static Common::Vec2  t_pos;
    static Common::Timer temp_time;

    static Common::Angle outFieldAng;
    static Common::Vec2  last_state_ball_pos;

    Common::logDebug("BEFORE: {}_{}", dmf, mid2);
    // wantThisRobot(attack);
    // wantThisRobot(dmf);
    //    positionRobots();//For lordhippo (after the first match with MRL)
    Common::logDebug("AFTER: {}_{}", dmf, mid2);

#if mark_in_stop
    MarkManager(false);

    for (std::map<int *, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i)
    {
        int opp = i->second;
        int own = *i->first;

        if (own == dmf)
        {
            continue;
        }

        if (opp == -1)
        {
            if (own == mid1)
            {
                ERRTSetObstacles(mid1, false, true);
                g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
                OwnRobot[mid1].face(m_world_state.ball.position);
                ERRTNavigate2Point(
                    mid1,
                    PointOnConnectingLine(
                        m_world_state.ball.position, Common::Vec2(side * Common::field().width, 0),
                        DIS(m_world_state.ball.position, Common::Vec2(side * Common::field().width, 0)) / 3.0f),
                    100, VelocityProfile::Type::Aroom);
            }
            else if (own == mid2)
            {
                ERRTSetObstacles(mid2, false, true);
                g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
                OwnRobot[mid2].face(m_world_state.ball.position);
                ERRTNavigate2Point(
                    mid2,
                    CircleAroundPoint(m_world_state.ball.position,
                                      NormalizeAngle(AngleWith(m_world_state.ball.position,
                                                               Common::Vec2(side * Common::field().width, 0))),
                                      1090),
                    100, VelocityProfile::Type::Aroom);
            }
        }
        else
        {
            Mark(own, opp, 500);
        }
    }

#else
    ERRTSetObstacles(mid1, false, true);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[mid1].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        mid1,
        m_world_state.ball.position.pointOnConnectingLine(
            Common::Vec2(side * Common::field().width, 0),
            m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(rw, false, true);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[rw].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        rw,
        Common::Vec2(0, -100) +
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(side * Common::field().width, 0),
                m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(lw, false, true);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[lw].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        lw,
        Common::Vec2(0, 100) +
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(side * Common::field().width, 0),
                m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(mid2, false, true);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[mid2].face(m_world_state.ball.position);
    ERRTNavigate2Point(mid2,
                       m_world_state.ball.position.circleAroundPoint(
                           m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)), 1090),
                       100, VelocityProfile::Type::Aroom);
#endif

    OwnRobot[attack].target.velocity.x = 0.0;
    OwnRobot[attack].target.velocity.y = 0.0;
    OwnRobot[dmf].target.velocity.x    = 0.0;
    OwnRobot[dmf].target.velocity.y    = 0.0;
    //    g_obs_map.resetMap();
    /*if(timer.time()>7 && FUNC_state < 2){//What the hell are you waiting for?!?!
        FUNC_state = 3;
        FUNC_CNT = 0;
    }*/
    if (FUNC_state == -2)
    {
        circle_ball_free(attack, outFieldAng, 24, 0, 0.0);

        if (last_state_ball_pos.distanceTo(m_world_state.ball.position) > 400)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 20)
        {
            FUNC_state = 3; // Back on track...
            FUNC_CNT   = 0;
            temp_time.start();
        }
    }
    else if (FUNC_state == -1)
    {
        if (outFieldAng.deg() == 0.0f)
        {
            if (m_world_state.ball.position.x > 0)
            {
                if (m_world_state.ball.position.y > 0)
                {
                    if (m_world_state.ball.position.x > Common::field().width)
                        outFieldAng.setDeg(155.0f);
                    else // So std::fabs(m_world_state.ball.position.y) > Common::field().height
                        outFieldAng.setDeg(-65.0f);
                }
                else
                {
                    if (m_world_state.ball.position.x > Common::field().width)
                        outFieldAng.setDeg(-155.0f);
                    else // So std::fabs(m_world_state.ball.position.y) > Common::field().height
                        outFieldAng.setDeg(65.0f);
                }
            }
            else
            {
                if (m_world_state.ball.position.y > 0)
                {
                    if (std::fabs(m_world_state.ball.position.x) > Common::field().width)
                        outFieldAng.setDeg(25.0f);
                    else // So std::fabs(m_world_state.ball.position.y) > Common::field().height
                        outFieldAng.setDeg(-115.0f);
                }
                else
                {
                    if (std::fabs(m_world_state.ball.position.x) > Common::field().width)
                        outFieldAng.setDeg(-25.0f);
                    else // So std::fabs(m_world_state.ball.position.y)  > Common::field().height
                        outFieldAng.setDeg(115.0f);
                }
            }
        }
        circle_ball_free(attack, outFieldAng, 0, 0, 0.0);
        Common::logDebug("outFieldAng: {}", outFieldAng.deg());
        Common::logDebug("OwnRobot[dmf].state().velocity.magnitude__{}", OwnRobot[dmf].state().velocity.length());

        if (OwnRobot[attack].state().velocity.length() < 20)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state          = -2;
            FUNC_CNT            = 0;
            last_state_ball_pos = m_world_state.ball.position;
        }
    }
    else if (FUNC_state == 0)
    {
        if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) < 95)
        {
            FUNC_state = 7;
            FUNC_CNT   = 0;
            t_ang.setDeg(0.0f);
            t_opp_ang.setDeg(180.0f);
        }
        else if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) < 10000.0f)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }
        else if (outOfField(m_world_state.ball.position))
        { // Do a little shoot on the wall
            FUNC_state = -1;
            FUNC_CNT   = 0;
            outFieldAng.setDeg(0.0f);
        }

        //        OwnRobot[attack].target.angle = temp_opp_ang;
        OwnRobot[dmf].target.angle = move_angle;
        //        ERRTSetObstacles(attack,0,0);
        //        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});
        //        ERRTNavigate2Point(attack, CircleAroundPoint(m_world_state.ball.position, move_angle, 250), 0,
        //        40, VelocityProfile::Type::Aroom);

        ERRTSetObstacles(dmf, 0, 0);
        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});
        ERRTNavigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(temp_opp_ang, 250), 40,
                           VelocityProfile::Type::Aroom);

        g_obs_map.resetMap();
        circle_ball_free(attack, move_angle, 0, 0, 0.0);
        Common::logDebug(":::{}", OwnRobot[attack].state().velocity.length());
        Common::logDebug("", OwnRobot[dmf].state().velocity.length());
        if (OwnRobot[attack].state().velocity.length() < 20 && OwnRobot[dmf].state().velocity.length() < 20)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state          = 1;
            FUNC_CNT            = 0;
            last_state_ball_pos = m_world_state.ball.position;
        }
    }
    else if (FUNC_state == 1)
    {
        circle_ball_free(attack, move_angle, 18, 0, 0.0);
        WaitForPass(dmf, false, &OwnRobot[attack].state().position);
        //        OwnRobot[dmf].target.angle = 90;
        if (last_state_ball_pos.distanceTo(m_world_state.ball.position) > 400)
        { // TODO added this TEST IT with the kicker on (already tested without the kicker)
            FUNC_CNT++;
        }
        if (OwnRobot[attack].state().position.distanceTo(m_world_state.ball.position) > 400)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 20)
        {
            FUNC_state = 2;
            FUNC_CNT   = 0;
            temp_time.start();
        }
    }
    else if (FUNC_state == 2)
    {
        WaitForPass(dmf, false, &OwnRobot[attack].state().position);
        //        OwnRobot[dmf].target.angle = 90;

        if (temp_time.time() > 2)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
            temp_time.start();
        }
    }
    else if (FUNC_state == 3)
    {
        if (std::fabs((OwnRobot[attack].target.angle - move_angle).deg()) > 90)
            std::swap(move_angle, temp_opp_ang);
        OwnRobot[attack].target.angle = move_angle;
        OwnRobot[dmf].target.angle    = temp_opp_ang;
        ERRTSetObstacles(attack, 0, 0);
        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});
        ERRTNavigate2Point(attack, m_world_state.ball.position.circleAroundPoint(temp_opp_ang, 250), 40,
                           VelocityProfile::Type::Aroom);
        ERRTSetObstacles(dmf, 0, 0);
        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});

        ERRTNavigate2Point(dmf, m_world_state.ball.position.circleAroundPoint(move_angle, 250), 40,
                           VelocityProfile::Type::Aroom);

        if (m_world_state.ball.position.circleAroundPoint(temp_opp_ang, 250)
                    .distanceTo(OwnRobot[attack].state().position) < 100 &&
            m_world_state.ball.position.circleAroundPoint(move_angle, 250).distanceTo(OwnRobot[dmf].state().position) <
                100 &&
            m_world_state.ball.velocity.length() < 10)
        {

            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 4;
            FUNC_CNT   = 0;
            t_pos      = m_world_state.ball.position;
        }
    }
    else if (FUNC_state == 4)
    {
        if (std::fabs((OwnRobot[attack].target.angle - move_angle).deg()) > 90)
            std::swap(move_angle, temp_opp_ang);

        //        if(AngleWith(OwnRobot[attack].state().position,OwnRobot[dmf].state().position))

        OwnRobot[attack].target.angle = move_angle;
        OwnRobot[dmf].target.angle    = temp_opp_ang;
        Navigate2Point(attack, t_pos.circleAroundPoint(temp_opp_ang, 75), 20,
                       VelocityProfile::Type::BallPlaceKheyliSooski);

        Navigate2Point(dmf, t_pos.circleAroundPoint(move_angle, 75), 20, VelocityProfile::Type::BallPlaceKheyliSooski);

        bool ball_has_slipt = ((OwnRobot[attack].state().position + OwnRobot[dmf].state().position) / 2)
                                  .distanceTo(m_world_state.ball.position) > 300;

        if (m_world_state.ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        if (t_pos.circleAroundPoint(temp_opp_ang, 75).distanceTo(OwnRobot[attack].state().position) < 40 &&
            t_pos.circleAroundPoint(move_angle, 75).distanceTo(OwnRobot[dmf].state().position) < 40)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 5;
            FUNC_CNT   = 0;
            t_ang      = move_angle;
            t_opp_ang  = temp_opp_ang;
        }
    }
    else if (FUNC_state == 5)
    {
        OwnRobot[attack].target.angle = t_ang;
        OwnRobot[dmf].target.angle    = t_opp_ang;
        Navigate2Point(attack, m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 75), 20,
                       VelocityProfile::Type::BallPlaceKheyliSooski);

        Navigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(t_ang, 75), 20,
                       VelocityProfile::Type::BallPlaceKheyliSooski);

        bool ball_has_slipt = ((OwnRobot[attack].state().position + OwnRobot[dmf].state().position) / 2)
                                  .distanceTo(m_world_state.ball.position) > 300;

        if (m_world_state.ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        if (m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 75)
                    .distanceTo(OwnRobot[attack].state().position) < 40 &&
            m_ref_state.place_ball_target.circleAroundPoint(t_ang, 75).distanceTo(OwnRobot[dmf].state().position) < 40)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 30)
        {
            FUNC_state = 6;
            FUNC_CNT   = 0;
        }
    }
    else if (FUNC_state == 6)
    {
        OwnRobot[attack].target.angle = t_ang;
        OwnRobot[dmf].target.angle    = t_opp_ang;
        // OwnRobot[attack].face(m_world_state.ball.position);
        Common::Vec2 target1 = m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 550);
        if (false && outOfField(target1))
        {
            Navigate2Point(attack, Common::Vec2(0, 0), 20, VelocityProfile::Type::BallPlaceKheyliSooski);
        }
        else
        {
            Navigate2Point(attack, target1, 20, VelocityProfile::Type::BallPlaceKheyliSooski);
        }
        // OwnRobot[dmf].face(m_world_state.ball.position);
        Common::Vec2 target2 = m_ref_state.place_ball_target.circleAroundPoint(t_ang, 550);

        if (false && outOfField(target1))
        {
            Navigate2Point(dmf, Common::Vec2(0, 0), 20, VelocityProfile::Type::BallPlaceKheyliSooski);
        }
        else
        {
            Navigate2Point(dmf, target2, 20, VelocityProfile::Type::BallPlaceKheyliSooski);
        }

        if (m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 550)
                    .distanceTo(OwnRobot[attack].state().position) < 40 &&
            m_ref_state.place_ball_target.circleAroundPoint(t_ang, 550).distanceTo(OwnRobot[dmf].state().position) < 40)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 30)
        {
            if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) > 95)
            {
                FUNC_state = 3;
                FUNC_CNT   = 0;
            }
            else
            {
                FUNC_state = 7;
                FUNC_CNT   = 0;
            }
        }
    }
    else
    {
        //        OwnRobot[attack].face(m_world_state.ball.position);
        //        Navigate2Point(attack,OwnRobot[attack].state().position,false,0,VelocityProfile::Type::BallPlaceKheyliSooski);
        OwnRobot[attack].face(m_world_state.ball.position); // TODO test this
        ERRTSetObstacles(attack, 1, 1);
        ERRTNavigate2Point(attack, m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 550), 20,
                           VelocityProfile::Type::BallPlaceKheyliSooski);
        OwnRobot[dmf].face(m_world_state.ball.position);
        ERRTSetObstacles(dmf, 1, 1);
        ERRTNavigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(t_ang, 550), 20,
                           VelocityProfile::Type::BallPlaceKheyliSooski);

        //        OwnRobot[attack].target.angle = 0;
        //        OwnRobot[dmf].target.angle = 0;
        //        t_ang = NormalizeAngle(OwnRobot[attack].state().angle + 180);
        //        Navigate2Point(attack, CircleAroundPoint(m_world_state.ball.position, t_ang, 170), 0, 20,
        //        VelocityProfile::Type::BallPlaceKheyliSooski); t_ang = NormalizeAngle(OwnRobot[dmf].state().angle +
        //        180); Navigate2Point(dmf, CircleAroundPoint(m_world_state.ball.position, t_ang, 170),  20,
        //        VelocityProfile::Type::BallPlaceKheyliSooski);

        bool success = m_ref_state.place_ball_target.distanceTo(m_world_state.ball.position) < 100.0;

        Common::logDebug("______IN___STATE_DONE_____");
        Common::logDebug("{}", m_ref_state.place_ball_target - m_world_state.ball.position);
        if (success)
        {
            Common::logInfo("MADE it!!!");
        }
        else
        {
            Common::logWarning("lost it!!!");
        }
    }
    Common::logInfo("______IN___STATE_{}_____", FUNC_state);

    Common::logInfo("___DIS___{}", m_ref_state.place_ball_target.distanceTo(m_world_state.ball.position));
    Common::logInfo("___BALL__POS_XY__{}, {}", m_world_state.ball.position.x, m_world_state.ball.position.y);
    Common::logInfo("___TARGET__POS_XY__{}, {}", m_ref_state.place_ball_target.x, m_ref_state.place_ball_target.y);
    Common::logInfo("__OUT__{}", outOfField(m_world_state.ball.position));
}

void Ai::our_place_ball_shoot_taki()
{

    GKHi(gk, true);
    DefBy3(def, rw, lw, nullptr, true);

    static Common::Angle move_angle, opp_ang;
    if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) > 150)
    {
        move_angle = m_ref_state.place_ball_target.angleWith(m_world_state.ball.position);
        opp_ang    = move_angle + Common::Angle::fromDeg(180);
    }

    static Common::Angle t_ang, t_opp_ang;
    static Common::Vec2  t_pos;
    static Common::Timer temp_time;

    Common::logDebug("BEFORE: {}_{}", dmf, mid2);
    wantThisRobot(attack);
    wantThisRobot(dmf);
    positionRobots();
    Common::logDebug("AFTER: {}_{}", dmf, mid2);

    ERRTSetObstacles(dmf, false, true);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[dmf].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        dmf,
        m_world_state.ball.position.pointOnConnectingLine(
            Common::Vec2(side * Common::field().width, 0),
            m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(rw, false, true);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[rw].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        rw,
        Common::Vec2(0, -100) +
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(side * Common::field().width, 0),
                m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(lw, false, true);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[lw].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        lw,
        Common::Vec2(0, 100) +
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(side * Common::field().width, 0),
                m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(mid2, false, true);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[mid2].face(m_world_state.ball.position);
    ERRTNavigate2Point(mid2,
                       m_world_state.ball.position.circleAroundPoint(
                           m_world_state.ball.position.angleWith(Common::Vec2(side * Common::field().width, 0)), 1090),
                       100, VelocityProfile::Type::Aroom);

    OwnRobot[attack].target.velocity.x = 0.0;
    OwnRobot[attack].target.velocity.y = 0.0;
    OwnRobot[dmf].target.velocity.x    = 0.0;
    OwnRobot[dmf].target.velocity.y    = 0.0;
    //    g_obs_map.resetMap();

    if (FUNC_state == 0)
    {
        if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) < 95)
        {
            FUNC_state = 7;
            FUNC_CNT   = 0;
            t_ang.setDeg(0);
            t_opp_ang.setDeg(180);
        }
        else if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) < 1000)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        //        OwnRobot[attack].target.angle = temp_opp_ang;
        OwnRobot[dmf].target.angle = move_angle;
        //        ERRTSetObstacles(attack,0,0);
        //        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});
        //        ERRTNavigate2Point(attack, CircleAroundPoint(m_world_state.ball.position, move_angle, 250), 0,
        //        40, VelocityProfile::Type::Aroom);

        ERRTSetObstacles(dmf, 0, 0);
        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});
        ERRTNavigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(opp_ang, 250), 40,
                           VelocityProfile::Type::Aroom);

        g_obs_map.resetMap();
        circle_ball_free(attack, move_angle, 0, 0, 0.0);
        Common::logDebug(":::{}", OwnRobot[attack].state().velocity.length());
        Common::logDebug("{}", OwnRobot[dmf].state().velocity.length());
        if (OwnRobot[attack].state().velocity.length() < 20 && OwnRobot[dmf].state().velocity.length() < 20)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 1;
            FUNC_CNT   = 0;
        }
    }
    else if (FUNC_state == 1)
    {
        circle_ball_free(attack, move_angle, 60, 0, 0.0);
        WaitForPass(dmf, false, &OwnRobot[attack].state().position);
        //        OwnRobot[dmf].target.angle = 90;
        if (OwnRobot[attack].state().position.distanceTo(m_world_state.ball.position) > 400)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 20)
        {
            FUNC_state = 2;
            FUNC_CNT   = 0;
            temp_time.start();
        }
    }
    else if (FUNC_state == 2)
    {
        WaitForPass(dmf, false, &OwnRobot[attack].state().position);
        //        OwnRobot[dmf].target.angle = 90;

        if (temp_time.time() > 2)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
            temp_time.start();
        }
    }
    else if (FUNC_state == 3)
    {
        //        OwnRobot[attack].target.angle = move_angle;
        //        ERRTSetObstacles(attack,0,0);
        //        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});
        float max_spd = 10.0f;
        if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) < 350)
            max_spd = 3.0f;
        //        circle_ball_free_V2(attack,move_angle,1,0,0,temp_vel);

        OwnRobot[attack].target.angle = move_angle;
        OwnRobot[dmf].target.angle    = opp_ang;
        Navigate2Point(attack, t_pos.circleAroundPoint(opp_ang, 75), max_spd,
                       VelocityProfile::Type::BallPlaceKheyliSooski);

        Navigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(move_angle, 75), 20,
                       VelocityProfile::Type::BallPlaceKheyliSooski);

        //        float angle_to_face =
        //        AngleWith(m_ref_state.place_ball_target,OwnRobot[attack].state().position);
        //        OwnRobot[dmf].target.angle = angle_to_face;
        //        ERRTSetObstacles(dmf,0,0);
        ////        TEMP_VEL_PRO.max_spd
        //        ERRTNavigate2Point(dmf, CircleAroundPoint(m_ref_state.place_ball_target, angle_to_face +
        //        180, 100), 0, 5, VelocityProfile::Type::BallPlaceKheyliSooski);

        if (m_world_state.ball.position.distanceTo(m_ref_state.place_ball_target) < 100 &&
            m_world_state.ball.velocity.length() < 10)
        {

            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 4;
            FUNC_CNT   = 0;
        }
    }
    else
    {
        //        OwnRobot[attack].face(m_world_state.ball.position);
        //        Navigate2Point(attack,OwnRobot[attack].state().position,false,0,VelocityProfile::Type::BallPlaceKheyliSooski);
        OwnRobot[attack].face(m_world_state.ball.position); // TODO test this
        ERRTSetObstacles(attack, 1, 1);
        ERRTNavigate2Point(attack, m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 550), 20,
                           VelocityProfile::Type::BallPlaceKheyliSooski);
        OwnRobot[dmf].face(m_world_state.ball.position);
        ERRTSetObstacles(dmf, 1, 1);
        ERRTNavigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(t_ang, 550), 20,
                           VelocityProfile::Type::BallPlaceKheyliSooski);

        //        OwnRobot[attack].target.angle = 0;
        //        OwnRobot[dmf].target.angle = 0;
        //        t_ang = NormalizeAngle(OwnRobot[attack].state().angle + 180);
        //        Navigate2Point(attack, CircleAroundPoint(m_world_state.ball.position, t_ang, 170), 0, 20,
        //        VelocityProfile::Type::BallPlaceKheyliSooski); t_ang = NormalizeAngle(OwnRobot[dmf].state().angle +
        //        180); Navigate2Point(dmf, CircleAroundPoint(m_world_state.ball.position, t_ang, 170),  20,
        //        VelocityProfile::Type::BallPlaceKheyliSooski);

        bool success = m_ref_state.place_ball_target.distanceTo(m_world_state.ball.position) < 100.0;

        Common::logDebug("______IN___STATE_DONE_____");
        Common::logDebug("{}", m_ref_state.place_ball_target - m_world_state.ball.position);
        if (success)
        {
            Common::logDebug("MADE it!!!");
        }
        else
        {
            Common::logWarning("lost it!!!");
        }
    }
    Common::logDebug("______IN___STATE_{}_____", FUNC_state);

    Common::logDebug("___DIS___{}", m_ref_state.place_ball_target.distanceTo(m_world_state.ball.position));
}
} // namespace Tyr::Soccer
