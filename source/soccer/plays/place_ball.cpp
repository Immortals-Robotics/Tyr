

#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::our_place_ball_shoot()
{

    GKHi(gk, true);
    DefMid(def, rw, lw, nullptr, true);

    static Common::Angle move_angle, temp_opp_ang;
    if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) > 100)
    {
        move_angle   = Common::refereeState().place_ball_target.angleWith(Common::worldState().ball.position);
        temp_opp_ang = Common::Angle::fromDeg(move_angle.deg() + 180.0f);
    }

    static Common::Angle   t_ang, t_opp_ang;
    static Common::Vec2    t_pos;
    static VelocityProfile TEMP_VEL_PRO = BALL_PLACE_KHEYLI_SOOSKI;
    static Common::Timer   temp_time;

    std::cout << "BEFORE: " << dmf << "_" << mid2 << std::endl;
    //    wantThisRobot(attack);
    //    wantThisRobot(dmf);
    positionRobots();
    std::cout << "AFTER: " << dmf << "_" << mid2 << std::endl;

    /*ERRTSetObstacles ( mid1 , false , true );
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[mid1].face(Common::worldState().ball.position);
    ERRTNavigate2Point ( mid1 , PointOnConnectingLine(Common::worldState().ball.position,
    Common::Vec2(side*Common::worldState().field.width, 0), DIS(Common::worldState().ball.position,
    Common::Vec2(side*Common::worldState().field.width, 0))/3.0f) ,0 , 100,&VELOCITY_PROFILE_AROOM);*/

    ERRTSetObstacles(rw, false, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[rw].face(Common::worldState().ball.position);
    ERRTNavigate2Point(rw,
                       Common::Vec2(0, -100) + Common::worldState().ball.position.pointOnConnectingLine(
                                                   Common::Vec2(side * Common::worldState().field.width, 0),
                                                   Common::worldState().ball.position.distanceTo(
                                                       Common::Vec2(side * Common::worldState().field.width, 0)) /
                                                       3.0f),
                       100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(lw, false, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[lw].face(Common::worldState().ball.position);
    ERRTNavigate2Point(lw,
                       Common::Vec2(0, 100) + Common::worldState().ball.position.pointOnConnectingLine(
                                                  Common::Vec2(side * Common::worldState().field.width, 0),
                                                  Common::worldState().ball.position.distanceTo(
                                                      Common::Vec2(side * Common::worldState().field.width, 0)) /
                                                      3.0f),
                       100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(mid2, false, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[mid2].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        mid2,
        Common::worldState().ball.position.circleAroundPoint(
            Common::worldState().ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)),
            1090),
        100, &VELOCITY_PROFILE_AROOM);

    OwnRobot[attack].target.velocity.x = 0.0;
    OwnRobot[attack].target.velocity.y = 0.0;
    OwnRobot[dmf].target.velocity.x    = 0.0;
    OwnRobot[dmf].target.velocity.y    = 0.0;
    //    obs_map.resetMap();
    /*if(timer.time()>7 && FUNC_state < 2){//What the hell are you waiting for?!?!
        FUNC_state = 3;
        FUNC_CNT = 0;
    }*/
    if (FUNC_state == 0)
    {
        if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) < 95)
        {
            FUNC_state = 7;
            FUNC_CNT   = 0;
            t_ang.setDeg(0.0f);
            t_opp_ang.setDeg(180.0f);
        }
        else if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) < 1000)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        //        OwnRobot[attack].target.angle = temp_opp_ang;
        OwnRobot[dmf].target.angle = move_angle;
        //        ERRTSetObstacles(attack,0,0);
        //        obs_map.addCircle({Common::worldState().ball.position, 150.0f});
        //        ERRTNavigate2Point(attack, CircleAroundPoint(Common::worldState().ball.position, move_angle, 250), 0,
        //        40, &VELOCITY_PROFILE_AROOM);

        ERRTSetObstacles(dmf, 0, 0);
        obs_map.addCircle({Common::worldState().ball.position, 150.0f});
        ERRTNavigate2Point(dmf, Common::refereeState().place_ball_target.circleAroundPoint(temp_opp_ang, 250), 40,
                           &VELOCITY_PROFILE_AROOM);

        obs_map.resetMap();
        circle_ball_free(attack, move_angle, 0, 0, 0.0);
        std::cout << ":::" << OwnRobot[attack].State.velocity.length() << std::endl
                  << OwnRobot[dmf].State.velocity.length() << std::endl;
        if (OwnRobot[attack].State.velocity.length() < 20 && OwnRobot[dmf].State.velocity.length() < 20)
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
        WaitForPass(dmf, false, &OwnRobot[attack].State.position);
        //        OwnRobot[dmf].target.angle = 90;
        if (OwnRobot[attack].State.position.distanceTo(Common::worldState().ball.position) > 400)
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

        WaitForPass(dmf, false, &OwnRobot[attack].State.position);
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
        obs_map.addCircle({Common::worldState().ball.position, 150.0f});
        ERRTNavigate2Point(attack, Common::worldState().ball.position.circleAroundPoint(temp_opp_ang, 250), 40,
                           &VELOCITY_PROFILE_AROOM);
        ERRTSetObstacles(dmf, 0, 0);
        obs_map.addCircle({Common::worldState().ball.position, 150.0f});

        ERRTNavigate2Point(dmf, Common::worldState().ball.position.circleAroundPoint(move_angle, 250), 40,
                           &VELOCITY_PROFILE_AROOM);

        if (Common::worldState()
                    .ball.position.circleAroundPoint(temp_opp_ang, 250)
                    .distanceTo(OwnRobot[attack].State.position) < 100 &&
            Common::worldState()
                    .ball.position.circleAroundPoint(move_angle, 250)
                    .distanceTo(OwnRobot[dmf].State.position) < 100 &&
            Common::worldState().ball.velocity.length() < 10)
        {

            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 4;
            FUNC_CNT   = 0;
            t_pos      = Common::worldState().ball.position;
        }
    }
    else if (FUNC_state == 4)
    {
        if (std::fabs((OwnRobot[attack].target.angle - move_angle).deg()) > 90)
            std::swap(move_angle, temp_opp_ang);

        //        if(AngleWith(OwnRobot[attack].State.position,OwnRobot[dmf].State.position))

        OwnRobot[attack].target.angle = move_angle;
        OwnRobot[dmf].target.angle    = temp_opp_ang;
        Navigate2Point(attack, t_pos.circleAroundPoint(temp_opp_ang, 75), 20, &BALL_PLACE_KHEYLI_SOOSKI);

        Navigate2Point(dmf, t_pos.circleAroundPoint(move_angle, 75), 20, &BALL_PLACE_KHEYLI_SOOSKI);

        bool ball_has_slipt = ((OwnRobot[attack].State.position + OwnRobot[dmf].State.position) / 2)
                                  .distanceTo(Common::worldState().ball.position) > 300;

        if (Common::worldState().ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        if (t_pos.circleAroundPoint(temp_opp_ang, 75).distanceTo(OwnRobot[attack].State.position) < 40 &&
            t_pos.circleAroundPoint(move_angle, 75).distanceTo(OwnRobot[dmf].State.position) < 40)
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
        Navigate2Point(attack, Common::refereeState().place_ball_target.circleAroundPoint(t_opp_ang, 75), 20,
                       &BALL_PLACE_KHEYLI_SOOSKI);

        Navigate2Point(dmf, Common::refereeState().place_ball_target.circleAroundPoint(t_ang, 75), 20,
                       &BALL_PLACE_KHEYLI_SOOSKI);

        bool ball_has_slipt = ((OwnRobot[attack].State.position + OwnRobot[dmf].State.position) / 2)
                                  .distanceTo(Common::worldState().ball.position) > 300;

        if (Common::worldState().ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        if (Common::refereeState()
                    .place_ball_target.circleAroundPoint(t_opp_ang, 75)
                    .distanceTo(OwnRobot[attack].State.position) < 40 &&
            Common::refereeState().place_ball_target.circleAroundPoint(t_ang, 75).distanceTo(
                OwnRobot[dmf].State.position) < 40)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 30)
        {
            FUNC_state           = 6;
            FUNC_CNT             = 0;
            TEMP_VEL_PRO.max_spd = Common::Vec2(300);
        }
    }
    else if (FUNC_state == 6)
    {

        OwnRobot[attack].target.angle = t_ang;
        OwnRobot[dmf].target.angle    = t_opp_ang;
        // OwnRobot[attack].face(Common::worldState().ball.position);
        Navigate2Point(attack, Common::refereeState().place_ball_target.circleAroundPoint(t_opp_ang, 250), 20,
                       &TEMP_VEL_PRO);
        // OwnRobot[dmf].face(Common::worldState().ball.position);
        Navigate2Point(dmf, Common::refereeState().place_ball_target.circleAroundPoint(t_ang, 250), 20, &TEMP_VEL_PRO);

        if (Common::refereeState()
                    .place_ball_target.circleAroundPoint(t_opp_ang, 250)
                    .distanceTo(OwnRobot[attack].State.position) < 40 &&
            Common::refereeState()
                    .place_ball_target.circleAroundPoint(t_ang, 250)
                    .distanceTo(OwnRobot[dmf].State.position) < 40)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 30)
        {
            if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) > 95)
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
        //        OwnRobot[attack].face(Common::worldState().ball.position);
        //        Navigate2Point(attack,OwnRobot[attack].State.position,false,0,&BALL_PLACE_KHEYLI_SOOSKI);
        OwnRobot[attack].face(Common::worldState().ball.position); // TODO test this
        ERRTSetObstacles(attack, 1, 1);
        ERRTNavigate2Point(attack, Common::refereeState().place_ball_target.circleAroundPoint(t_opp_ang, 550), 20,
                           &BALL_PLACE_KHEYLI_SOOSKI);
        OwnRobot[dmf].face(Common::worldState().ball.position);
        ERRTSetObstacles(dmf, 1, 1);
        ERRTNavigate2Point(dmf, Common::refereeState().place_ball_target.circleAroundPoint(t_ang, 550), 20,
                           &BALL_PLACE_KHEYLI_SOOSKI);

        //        OwnRobot[attack].target.angle = 0;
        //        OwnRobot[dmf].target.angle = 0;
        //        t_ang = NormalizeAngle(OwnRobot[attack].State.angle + 180);
        //        Navigate2Point(attack, CircleAroundPoint(Common::worldState().ball.position, t_ang, 170), 0, 20,
        //        &BALL_PLACE_KHEYLI_SOOSKI); t_ang = NormalizeAngle(OwnRobot[dmf].State.angle + 180);
        //        Navigate2Point(dmf, CircleAroundPoint(Common::worldState().ball.position, t_ang, 170),  20,
        //        &BALL_PLACE_KHEYLI_SOOSKI);

        bool success = Common::refereeState().place_ball_target.distanceTo(Common::worldState().ball.position) < 100.0;

        std::cout << "______IN___STATE_DONE_____" << std::endl
                  << Common::refereeState().place_ball_target.x - Common::worldState().ball.position.x << std::endl
                  << Common::refereeState().place_ball_target.y - Common::worldState().ball.position.y << std::endl;
        if (success)
        {
            std::cout << "MADE it!!!" << std::endl;
        }
        else
        {
            std::cout << "lost it!!!" << std::endl;
        }
    }
    std::cout << "______IN___STATE_" << FUNC_state << "_____" << std::endl;

    std::cout << "___DIS___" << Common::refereeState().place_ball_target.distanceTo(Common::worldState().ball.position)
              << std::endl;
}

void Ai::our_place_ball_shoot_V2()
{

    GKHi(gk, true);
    DefMid(def, rw, lw, nullptr, true);

    static Common::Angle move_angle, temp_opp_ang;
    if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) > 100)
    {
        move_angle   = Common::refereeState().place_ball_target.angleWith(Common::worldState().ball.position);
        temp_opp_ang = move_angle + Common::Angle::fromDeg(180.0f);
    }

    static Common::Angle   t_ang, t_opp_ang;
    static Common::Vec2    t_pos;
    static VelocityProfile TEMP_VEL_PRO = BALL_PLACE_KHEYLI_SOOSKI;
    static Common::Timer   temp_time;

    static Common::Angle outFieldAng;
    static Common::Vec2  last_state_ball_pos;

    std::cout << "BEFORE: " << dmf << "_" << mid2 << std::endl;
    // wantThisRobot(attack);
    // wantThisRobot(dmf);
    //    positionRobots();//For lordhippo (after the first match with MRL)
    std::cout << "AFTER: " << dmf << "_" << mid2 << std::endl;

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
                obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
                OwnRobot[mid1].face(Common::worldState().ball.position);
                ERRTNavigate2Point(mid1,
                                   PointOnConnectingLine(Common::worldState().ball.position,
                                                         Common::Vec2(side * Common::worldState().field.width, 0),
                                                         DIS(Common::worldState().ball.position,
                                                             Common::Vec2(side * Common::worldState().field.width, 0)) /
                                                             3.0f),
                                   100, &VELOCITY_PROFILE_AROOM);
            }
            else if (own == mid2)
            {
                ERRTSetObstacles(mid2, false, true);
                obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
                OwnRobot[mid2].face(Common::worldState().ball.position);
                ERRTNavigate2Point(mid2,
                                   CircleAroundPoint(Common::worldState().ball.position,
                                                     NormalizeAngle(AngleWith(
                                                         Common::worldState().ball.position,
                                                         Common::Vec2(side * Common::worldState().field.width, 0))),
                                                     1090),
                                   100, &VELOCITY_PROFILE_AROOM);
            }
        }
        else
        {
            Mark(own, opp, 500);
        }
    }

#else
    ERRTSetObstacles(mid1, false, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[mid1].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        mid1,
        Common::worldState().ball.position.pointOnConnectingLine(
            Common::Vec2(side * Common::worldState().field.width, 0),
            Common::worldState().ball.position.distanceTo(Common::Vec2(side * Common::worldState().field.width, 0)) /
                3.0f),
        100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(rw, false, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[rw].face(Common::worldState().ball.position);
    ERRTNavigate2Point(rw,
                       Common::Vec2(0, -100) + Common::worldState().ball.position.pointOnConnectingLine(
                                                   Common::Vec2(side * Common::worldState().field.width, 0),
                                                   Common::worldState().ball.position.distanceTo(
                                                       Common::Vec2(side * Common::worldState().field.width, 0)) /
                                                       3.0f),
                       100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(lw, false, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[lw].face(Common::worldState().ball.position);
    ERRTNavigate2Point(lw,
                       Common::Vec2(0, 100) + Common::worldState().ball.position.pointOnConnectingLine(
                                                  Common::Vec2(side * Common::worldState().field.width, 0),
                                                  Common::worldState().ball.position.distanceTo(
                                                      Common::Vec2(side * Common::worldState().field.width, 0)) /
                                                      3.0f),
                       100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(mid2, false, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[mid2].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        mid2,
        Common::worldState().ball.position.circleAroundPoint(
            Common::worldState().ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)),
            1090),
        100, &VELOCITY_PROFILE_AROOM);
#endif

    OwnRobot[attack].target.velocity.x = 0.0;
    OwnRobot[attack].target.velocity.y = 0.0;
    OwnRobot[dmf].target.velocity.x    = 0.0;
    OwnRobot[dmf].target.velocity.y    = 0.0;
    //    obs_map.resetMap();
    /*if(timer.time()>7 && FUNC_state < 2){//What the hell are you waiting for?!?!
        FUNC_state = 3;
        FUNC_CNT = 0;
    }*/
    if (FUNC_state == -2)
    {
        circle_ball_free(attack, outFieldAng, 24, 0, 0.0);

        if (last_state_ball_pos.distanceTo(Common::worldState().ball.position) > 400)
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
            if (Common::worldState().ball.position.x > 0)
            {
                if (Common::worldState().ball.position.y > 0)
                {
                    if (Common::worldState().ball.position.x > Common::worldState().field.width)
                        outFieldAng.setDeg(155.0f);
                    else // So std::fabs(Common::worldState().ball.position.y) > Common::worldState().field.height
                        outFieldAng.setDeg(-65.0f);
                }
                else
                {
                    if (Common::worldState().ball.position.x > Common::worldState().field.width)
                        outFieldAng.setDeg(-155.0f);
                    else // So std::fabs(Common::worldState().ball.position.y) > Common::worldState().field.height
                        outFieldAng.setDeg(65.0f);
                }
            }
            else
            {
                if (Common::worldState().ball.position.y > 0)
                {
                    if (std::fabs(Common::worldState().ball.position.x) > Common::worldState().field.width)
                        outFieldAng.setDeg(25.0f);
                    else // So std::fabs(Common::worldState().ball.position.y) > Common::worldState().field.height
                        outFieldAng.setDeg(-115.0f);
                }
                else
                {
                    if (std::fabs(Common::worldState().ball.position.x) > Common::worldState().field.width)
                        outFieldAng.setDeg(-25.0f);
                    else // So std::fabs(Common::worldState().ball.position.y)  > Common::worldState().field.height
                        outFieldAng.setDeg(115.0f);
                }
            }
        }
        circle_ball_free(attack, outFieldAng, 0, 0, 0.0);
        std::cout << "outFieldAng: " << outFieldAng.deg() << std::endl;
        std::cout << "OwnRobot[dmf].State.velocity.magnitude__" << OwnRobot[dmf].State.velocity.length() << std::endl;

        if (OwnRobot[attack].State.velocity.length() < 20)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state          = -2;
            FUNC_CNT            = 0;
            last_state_ball_pos = Common::worldState().ball.position;
        }
    }
    else if (FUNC_state == 0)
    {
        if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) < 95)
        {
            FUNC_state = 7;
            FUNC_CNT   = 0;
            t_ang.setDeg(0.0f);
            t_opp_ang.setDeg(180.0f);
        }
        else if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) < 10000.0f)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }
        else if (outOfField(Common::worldState().ball.position))
        { // Do a little shoot on the wall
            FUNC_state = -1;
            FUNC_CNT   = 0;
            outFieldAng.setDeg(0.0f);
        }

        //        OwnRobot[attack].target.angle = temp_opp_ang;
        OwnRobot[dmf].target.angle = move_angle;
        //        ERRTSetObstacles(attack,0,0);
        //        obs_map.addCircle({Common::worldState().ball.position, 150.0f});
        //        ERRTNavigate2Point(attack, CircleAroundPoint(Common::worldState().ball.position, move_angle, 250), 0,
        //        40, &VELOCITY_PROFILE_AROOM);

        ERRTSetObstacles(dmf, 0, 0);
        obs_map.addCircle({Common::worldState().ball.position, 150.0f});
        ERRTNavigate2Point(dmf, Common::refereeState().place_ball_target.circleAroundPoint(temp_opp_ang, 250), 40,
                           &VELOCITY_PROFILE_AROOM);

        obs_map.resetMap();
        circle_ball_free(attack, move_angle, 0, 0, 0.0);
        std::cout << ":::" << OwnRobot[attack].State.velocity.length() << std::endl
                  << OwnRobot[dmf].State.velocity.length() << std::endl;
        if (OwnRobot[attack].State.velocity.length() < 20 && OwnRobot[dmf].State.velocity.length() < 20)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state          = 1;
            FUNC_CNT            = 0;
            last_state_ball_pos = Common::worldState().ball.position;
        }
    }
    else if (FUNC_state == 1)
    {

        circle_ball_free(attack, move_angle, 18, 0, 0.0);
        WaitForPass(dmf, false, &OwnRobot[attack].State.position);
        //        OwnRobot[dmf].target.angle = 90;
        if (last_state_ball_pos.distanceTo(Common::worldState().ball.position) > 400)
        { // TODO added this TEST IT with the kicker on (already tested without the kicker)
            FUNC_CNT++;
        }
        if (OwnRobot[attack].State.position.distanceTo(Common::worldState().ball.position) > 400)
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

        WaitForPass(dmf, false, &OwnRobot[attack].State.position);
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
        obs_map.addCircle({Common::worldState().ball.position, 150.0f});
        ERRTNavigate2Point(attack, Common::worldState().ball.position.circleAroundPoint(temp_opp_ang, 250), 40,
                           &VELOCITY_PROFILE_AROOM);
        ERRTSetObstacles(dmf, 0, 0);
        obs_map.addCircle({Common::worldState().ball.position, 150.0f});

        ERRTNavigate2Point(dmf, Common::worldState().ball.position.circleAroundPoint(move_angle, 250), 40,
                           &VELOCITY_PROFILE_AROOM);

        if (Common::worldState()
                    .ball.position.circleAroundPoint(temp_opp_ang, 250)
                    .distanceTo(OwnRobot[attack].State.position) < 100 &&
            Common::worldState()
                    .ball.position.circleAroundPoint(move_angle, 250)
                    .distanceTo(OwnRobot[dmf].State.position) < 100 &&
            Common::worldState().ball.velocity.length() < 10)
        {

            FUNC_CNT++;
        }
        if (FUNC_CNT >= 10)
        {
            FUNC_state = 4;
            FUNC_CNT   = 0;
            t_pos      = Common::worldState().ball.position;
        }
    }
    else if (FUNC_state == 4)
    {
        if (std::fabs((OwnRobot[attack].target.angle - move_angle).deg()) > 90)
            std::swap(move_angle, temp_opp_ang);

        //        if(AngleWith(OwnRobot[attack].State.position,OwnRobot[dmf].State.position))

        OwnRobot[attack].target.angle = move_angle;
        OwnRobot[dmf].target.angle    = temp_opp_ang;
        Navigate2Point(attack, t_pos.circleAroundPoint(temp_opp_ang, 75), 20, &BALL_PLACE_KHEYLI_SOOSKI);

        Navigate2Point(dmf, t_pos.circleAroundPoint(move_angle, 75), 20, &BALL_PLACE_KHEYLI_SOOSKI);

        bool ball_has_slipt = ((OwnRobot[attack].State.position + OwnRobot[dmf].State.position) / 2)
                                  .distanceTo(Common::worldState().ball.position) > 300;

        if (Common::worldState().ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        if (t_pos.circleAroundPoint(temp_opp_ang, 75).distanceTo(OwnRobot[attack].State.position) < 40 &&
            t_pos.circleAroundPoint(move_angle, 75).distanceTo(OwnRobot[dmf].State.position) < 40)
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
        Navigate2Point(attack, Common::refereeState().place_ball_target.circleAroundPoint(t_opp_ang, 75), 20,
                       &BALL_PLACE_KHEYLI_SOOSKI);

        Navigate2Point(dmf, Common::refereeState().place_ball_target.circleAroundPoint(t_ang, 75), 20,
                       &BALL_PLACE_KHEYLI_SOOSKI);

        bool ball_has_slipt = ((OwnRobot[attack].State.position + OwnRobot[dmf].State.position) / 2)
                                  .distanceTo(Common::worldState().ball.position) > 300;

        if (Common::worldState().ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        if (Common::refereeState()
                    .place_ball_target.circleAroundPoint(t_opp_ang, 75)
                    .distanceTo(OwnRobot[attack].State.position) < 40 &&
            Common::refereeState().place_ball_target.circleAroundPoint(t_ang, 75).distanceTo(
                OwnRobot[dmf].State.position) < 40)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 30)
        {
            FUNC_state           = 6;
            FUNC_CNT             = 0;
            TEMP_VEL_PRO.max_spd = Common::Vec2(300);
        }
    }
    else if (FUNC_state == 6)
    {

        OwnRobot[attack].target.angle = t_ang;
        OwnRobot[dmf].target.angle    = t_opp_ang;
        // OwnRobot[attack].face(Common::worldState().ball.position);
        Common::Vec2 target1 = Common::refereeState().place_ball_target.circleAroundPoint(t_opp_ang, 550);
        if (false && outOfField(target1))
        {
            Navigate2Point(attack, Common::Vec2(0, 0), 20, &TEMP_VEL_PRO);
        }
        else
        {
            Navigate2Point(attack, target1, 20, &TEMP_VEL_PRO);
        }
        // OwnRobot[dmf].face(Common::worldState().ball.position);
        Common::Vec2 target2 = Common::refereeState().place_ball_target.circleAroundPoint(t_ang, 550);

        if (false && outOfField(target1))
        {
            Navigate2Point(dmf, Common::Vec2(0, 0), 20, &TEMP_VEL_PRO);
        }
        else
        {
            Navigate2Point(dmf, target2, 20, &TEMP_VEL_PRO);
        }

        if (Common::refereeState()
                    .place_ball_target.circleAroundPoint(t_opp_ang, 550)
                    .distanceTo(OwnRobot[attack].State.position) < 40 &&
            Common::refereeState()
                    .place_ball_target.circleAroundPoint(t_ang, 550)
                    .distanceTo(OwnRobot[dmf].State.position) < 40)
        {
            FUNC_CNT++;
        }
        if (FUNC_CNT >= 30)
        {
            if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) > 95)
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
        //        OwnRobot[attack].face(Common::worldState().ball.position);
        //        Navigate2Point(attack,OwnRobot[attack].State.position,false,0,&BALL_PLACE_KHEYLI_SOOSKI);
        OwnRobot[attack].face(Common::worldState().ball.position); // TODO test this
        ERRTSetObstacles(attack, 1, 1);
        ERRTNavigate2Point(attack, Common::refereeState().place_ball_target.circleAroundPoint(t_opp_ang, 550), 20,
                           &BALL_PLACE_KHEYLI_SOOSKI);
        OwnRobot[dmf].face(Common::worldState().ball.position);
        ERRTSetObstacles(dmf, 1, 1);
        ERRTNavigate2Point(dmf, Common::refereeState().place_ball_target.circleAroundPoint(t_ang, 550), 20,
                           &BALL_PLACE_KHEYLI_SOOSKI);

        //        OwnRobot[attack].target.angle = 0;
        //        OwnRobot[dmf].target.angle = 0;
        //        t_ang = NormalizeAngle(OwnRobot[attack].State.angle + 180);
        //        Navigate2Point(attack, CircleAroundPoint(Common::worldState().ball.position, t_ang, 170), 0, 20,
        //        &BALL_PLACE_KHEYLI_SOOSKI); t_ang = NormalizeAngle(OwnRobot[dmf].State.angle + 180);
        //        Navigate2Point(dmf, CircleAroundPoint(Common::worldState().ball.position, t_ang, 170),  20,
        //        &BALL_PLACE_KHEYLI_SOOSKI);

        bool success = Common::refereeState().place_ball_target.distanceTo(Common::worldState().ball.position) < 100.0;

        std::cout << "______IN___STATE_DONE_____" << std::endl
                  << Common::refereeState().place_ball_target.x - Common::worldState().ball.position.x << std::endl
                  << Common::refereeState().place_ball_target.y - Common::worldState().ball.position.y << std::endl;
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

    Common::logInfo("___DIS___{}",
                    Common::refereeState().place_ball_target.distanceTo(Common::worldState().ball.position));
    Common::logInfo("___BALL__POS_XY__{}, {}", Common::worldState().ball.position.x,
                    Common::worldState().ball.position.y);
    Common::logInfo("___TARGET__POS_XY__{}, {}", Common::refereeState().place_ball_target.x,
                    Common::refereeState().place_ball_target.y);
    Common::logInfo("__OUT__{}", outOfField(Common::worldState().ball.position));
}

void Ai::our_place_ball_shoot_taki()
{

    GKHi(gk, true);
    DefMid(def, rw, lw, nullptr, true);

    static Common::Angle move_angle, opp_ang;
    if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) > 150)
    {
        move_angle = Common::refereeState().place_ball_target.angleWith(Common::worldState().ball.position);
        opp_ang    = move_angle + Common::Angle::fromDeg(180);
    }

    static Common::Angle   t_ang, t_opp_ang;
    static Common::Vec2    t_pos;
    static VelocityProfile TEMP_VEL_PRO = BALL_PLACE_KHEYLI_SOOSKI;
    static Common::Timer   temp_time;

    std::cout << "BEFORE: " << dmf << "_" << mid2 << std::endl;
    wantThisRobot(attack);
    wantThisRobot(dmf);
    positionRobots();
    std::cout << "AFTER: " << dmf << "_" << mid2 << std::endl;

    ERRTSetObstacles(dmf, false, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[dmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        dmf,
        Common::worldState().ball.position.pointOnConnectingLine(
            Common::Vec2(side * Common::worldState().field.width, 0),
            Common::worldState().ball.position.distanceTo(Common::Vec2(side * Common::worldState().field.width, 0)) /
                3.0f),
        100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(rw, false, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[rw].face(Common::worldState().ball.position);
    ERRTNavigate2Point(rw,
                       Common::Vec2(0, -100) + Common::worldState().ball.position.pointOnConnectingLine(
                                                   Common::Vec2(side * Common::worldState().field.width, 0),
                                                   Common::worldState().ball.position.distanceTo(
                                                       Common::Vec2(side * Common::worldState().field.width, 0)) /
                                                       3.0f),
                       100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(lw, false, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[lw].face(Common::worldState().ball.position);
    ERRTNavigate2Point(lw,
                       Common::Vec2(0, 100) + Common::worldState().ball.position.pointOnConnectingLine(
                                                  Common::Vec2(side * Common::worldState().field.width, 0),
                                                  Common::worldState().ball.position.distanceTo(
                                                      Common::Vec2(side * Common::worldState().field.width, 0)) /
                                                      3.0f),
                       100, &VELOCITY_PROFILE_AROOM);

    ERRTSetObstacles(mid2, false, true);
    obs_map.addCircle({Common::worldState().ball.position, 1010.0f});
    OwnRobot[mid2].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        mid2,
        Common::worldState().ball.position.circleAroundPoint(
            Common::worldState().ball.position.angleWith(Common::Vec2(side * Common::worldState().field.width, 0)),
            1090),
        100, &VELOCITY_PROFILE_AROOM);

    OwnRobot[attack].target.velocity.x = 0.0;
    OwnRobot[attack].target.velocity.y = 0.0;
    OwnRobot[dmf].target.velocity.x    = 0.0;
    OwnRobot[dmf].target.velocity.y    = 0.0;
    //    obs_map.resetMap();

    if (FUNC_state == 0)
    {
        if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) < 95)
        {
            FUNC_state = 7;
            FUNC_CNT   = 0;
            t_ang.setDeg(0);
            t_opp_ang.setDeg(180);
        }
        else if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) < 1000)
        {
            FUNC_state = 3;
            FUNC_CNT   = 0;
        }

        //        OwnRobot[attack].target.angle = temp_opp_ang;
        OwnRobot[dmf].target.angle = move_angle;
        //        ERRTSetObstacles(attack,0,0);
        //        obs_map.addCircle({Common::worldState().ball.position, 150.0f});
        //        ERRTNavigate2Point(attack, CircleAroundPoint(Common::worldState().ball.position, move_angle, 250), 0,
        //        40, &VELOCITY_PROFILE_AROOM);

        ERRTSetObstacles(dmf, 0, 0);
        obs_map.addCircle({Common::worldState().ball.position, 150.0f});
        ERRTNavigate2Point(dmf, Common::refereeState().place_ball_target.circleAroundPoint(opp_ang, 250), 40,
                           &VELOCITY_PROFILE_AROOM);

        obs_map.resetMap();
        circle_ball_free(attack, move_angle, 0, 0, 0.0);
        std::cout << ":::" << OwnRobot[attack].State.velocity.length() << std::endl
                  << OwnRobot[dmf].State.velocity.length() << std::endl;
        if (OwnRobot[attack].State.velocity.length() < 20 && OwnRobot[dmf].State.velocity.length() < 20)
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
        WaitForPass(dmf, false, &OwnRobot[attack].State.position);
        //        OwnRobot[dmf].target.angle = 90;
        if (OwnRobot[attack].State.position.distanceTo(Common::worldState().ball.position) > 400)
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

        WaitForPass(dmf, false, &OwnRobot[attack].State.position);
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
        //        obs_map.addCircle({Common::worldState().ball.position, 150.0f});
        VelocityProfile temp_vel = BALL_PLACE_KHEYLI_SOOSKI;
        temp_vel.max_spd         = Common::Vec2(10.0);
        if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) < 350)
            temp_vel.max_spd = Common::Vec2(3.0);
        //        circle_ball_free_V2(attack,move_angle,1,0,0,temp_vel);

        OwnRobot[attack].target.angle = move_angle;
        OwnRobot[dmf].target.angle    = opp_ang;
        Navigate2Point(attack, t_pos.circleAroundPoint(opp_ang, 75), 20, &temp_vel);

        Navigate2Point(dmf, Common::refereeState().place_ball_target.circleAroundPoint(move_angle, 75), 20,
                       &BALL_PLACE_KHEYLI_SOOSKI);

        //        float angle_to_face =
        //        AngleWith(Common::refereeState().place_ball_target,OwnRobot[attack].State.position);
        //        OwnRobot[dmf].target.angle = angle_to_face;
        //        ERRTSetObstacles(dmf,0,0);
        ////        TEMP_VEL_PRO.max_spd
        //        ERRTNavigate2Point(dmf, CircleAroundPoint(Common::refereeState().place_ball_target, angle_to_face +
        //        180, 100), 0, 5, &BALL_PLACE_KHEYLI_SOOSKI);

        if (Common::worldState().ball.position.distanceTo(Common::refereeState().place_ball_target) < 100 &&
            Common::worldState().ball.velocity.length() < 10)
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
        //        OwnRobot[attack].face(Common::worldState().ball.position);
        //        Navigate2Point(attack,OwnRobot[attack].State.position,false,0,&BALL_PLACE_KHEYLI_SOOSKI);
        OwnRobot[attack].face(Common::worldState().ball.position); // TODO test this
        ERRTSetObstacles(attack, 1, 1);
        ERRTNavigate2Point(attack, Common::refereeState().place_ball_target.circleAroundPoint(t_opp_ang, 550), 20,
                           &BALL_PLACE_KHEYLI_SOOSKI);
        OwnRobot[dmf].face(Common::worldState().ball.position);
        ERRTSetObstacles(dmf, 1, 1);
        ERRTNavigate2Point(dmf, Common::refereeState().place_ball_target.circleAroundPoint(t_ang, 550), 20,
                           &BALL_PLACE_KHEYLI_SOOSKI);

        //        OwnRobot[attack].target.angle = 0;
        //        OwnRobot[dmf].target.angle = 0;
        //        t_ang = NormalizeAngle(OwnRobot[attack].State.angle + 180);
        //        Navigate2Point(attack, CircleAroundPoint(Common::worldState().ball.position, t_ang, 170), 0, 20,
        //        &BALL_PLACE_KHEYLI_SOOSKI); t_ang = NormalizeAngle(OwnRobot[dmf].State.angle + 180);
        //        Navigate2Point(dmf, CircleAroundPoint(Common::worldState().ball.position, t_ang, 170),  20,
        //        &BALL_PLACE_KHEYLI_SOOSKI);

        bool success = Common::refereeState().place_ball_target.distanceTo(Common::worldState().ball.position) < 100.0;

        std::cout << "______IN___STATE_DONE_____" << std::endl
                  << Common::refereeState().place_ball_target.x - Common::worldState().ball.position.x << std::endl
                  << Common::refereeState().place_ball_target.y - Common::worldState().ball.position.y << std::endl;
        if (success)
        {
            std::cout << "MADE it!!!" << std::endl;
        }
        else
        {
            std::cout << "lost it!!!" << std::endl;
        }
    }
    std::cout << "______IN___STATE_" << FUNC_state << "_____" << std::endl;

    std::cout << "___DIS___" << Common::refereeState().place_ball_target.distanceTo(Common::worldState().ball.position)
              << std::endl;
}
} // namespace Tyr::Soccer
