

#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::our_place_ball()
{
    GKHi(gk, true);
    DefHi(def, rw, lw, nullptr, true);

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

    ERRTSetObstacles(mid1);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[mid1].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        mid1,
        m_world_state.ball.position.pointOnConnectingLine(
            Common::Vec2(side * Common::field().width, 0),
            m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(rw);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[rw].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        rw,
        Common::Vec2(0, -100) +
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(side * Common::field().width, 0),
                m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(lw);
    g_obs_map.addCircle({m_world_state.ball.position, 1010.0f});
    OwnRobot[lw].face(m_world_state.ball.position);
    ERRTNavigate2Point(
        lw,
        Common::Vec2(0, 100) +
            m_world_state.ball.position.pointOnConnectingLine(
                Common::Vec2(side * Common::field().width, 0),
                m_world_state.ball.position.distanceTo(Common::Vec2(side * Common::field().width, 0)) / 3.0f),
        100, VelocityProfile::Type::Aroom);

    ERRTSetObstacles(mid2);
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

    if (FUNC_state == -2)
    {
        circle_ball(attack, outFieldAng, 24, 0, 0.0);

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
        circle_ball(attack, outFieldAng, 0, 0, 0.0);
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

        OwnRobot[dmf].target.angle = move_angle;

        ERRTSetObstacles(dmf);
        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});
        ERRTNavigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(temp_opp_ang, 250), 40,
                           VelocityProfile::Type::Aroom);

        g_obs_map.resetMap();
        circle_ball(attack, move_angle, 0, 0, 0.0);
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
        circle_ball(attack, move_angle, 18, 0, 0.0);
        WaitForPass(dmf, false, &OwnRobot[attack].state().position);

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
        ERRTSetObstacles(attack);
        g_obs_map.addCircle({m_world_state.ball.position, 150.0f});
        ERRTNavigate2Point(attack, m_world_state.ball.position.circleAroundPoint(temp_opp_ang, 250), 40,
                           VelocityProfile::Type::Aroom);
        ERRTSetObstacles(dmf);
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
        Common::Vec2 target1          = m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 550);
        if (false && outOfField(target1))
        {
            Navigate2Point(attack, Common::Vec2(0, 0), 20, VelocityProfile::Type::BallPlaceKheyliSooski);
        }
        else
        {
            Navigate2Point(attack, target1, 20, VelocityProfile::Type::BallPlaceKheyliSooski);
        }
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
        OwnRobot[attack].face(m_world_state.ball.position); // TODO test this
        ERRTSetObstacles(attack, true);
        ERRTNavigate2Point(attack, m_ref_state.place_ball_target.circleAroundPoint(t_opp_ang, 550), 20,
                           VelocityProfile::Type::BallPlaceKheyliSooski);
        OwnRobot[dmf].face(m_world_state.ball.position);
        ERRTSetObstacles(dmf, true);
        ERRTNavigate2Point(dmf, m_ref_state.place_ball_target.circleAroundPoint(t_ang, 550), 20,
                           VelocityProfile::Type::BallPlaceKheyliSooski);

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
} // namespace Tyr::Soccer
