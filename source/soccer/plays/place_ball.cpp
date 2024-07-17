

#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::ourPlaceBall()
{
    gkHi(m_gk);
    defHi(m_def1, m_def2, nullptr);

    static Common::Angle move_angle, temp_opp_ang;
    if (m_world_state.ball.position.distanceTo(m_ref_state.designated_position) > 100)
    {
        move_angle   = m_ref_state.designated_position.angleWith(m_world_state.ball.position);
        temp_opp_ang = move_angle + Common::Angle::fromDeg(180.0f);
    }

    static Common::Angle t_ang, t_opp_ang;
    static Common::Vec2  t_pos;
    static Common::Timer temp_time;

    static Common::Angle outFieldAng;
    static Common::Vec2  last_state_ball_pos;

    m_own_robot[m_mid1].face(m_world_state.ball.position);
    navigate(m_mid1,
             m_world_state.ball.position.pointOnConnectingLine(
                 ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
             VelocityProfile::aroom(), NavigationFlagsForceBallObstacle);

    m_own_robot[m_mid2].face(m_world_state.ball.position);
    navigate(m_mid2,
             m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 1090),
             VelocityProfile::aroom(), NavigationFlagsForceBallObstacle);

    if (m_func_state == -2)
    {
        circleBall(m_attack, outFieldAng, 24, 0);

        // TODO: transition when m_dmf fits behind the ball
        if (last_state_ball_pos.distanceTo(m_world_state.ball.position) > 400)
        {
            m_func_count++;
        }
        if (m_func_count >= 20)
        {
            m_func_state = 3; // Back on track...
            m_func_count = 0;
            temp_time.start();
        }
    }
    else if (m_func_state == -1)
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
        circleBall(m_attack, outFieldAng, 0, 0);
        Common::logDebug("outFieldAng: {}", outFieldAng.deg());

        if (m_own_robot[m_attack].state().velocity.length() < 20)
        {
            m_func_count++;
        }
        if (m_func_count >= 10)
        {
            m_func_state        = -2;
            m_func_count        = 0;
            last_state_ball_pos = m_world_state.ball.position;
        }
    }
    else if (m_func_state == 0)
    {
        if (m_world_state.ball.position.distanceTo(m_ref_state.designated_position) < 95)
        {
            m_func_state = 7;
            m_func_count = 0;
            t_ang.setDeg(0.0f);
            t_opp_ang.setDeg(180.0f);
        }
        // TODO: only do this when m_dmf doesn't fit behind the ball
        else if (isOut(m_world_state.ball.position))
        { // Do a little shoot on the wall
            m_func_state = -1;
            m_func_count = 0;
            outFieldAng.setDeg(0.0f);
        }
        else if (m_world_state.ball.position.distanceTo(m_ref_state.designated_position) < 10000.0f)
        {
            m_func_state = 3;
            m_func_count = 0;
        }

        m_own_robot[m_mid5].target.angle = move_angle;

        navigate(m_mid5, m_ref_state.designated_position.circleAroundPoint(temp_opp_ang, 250), VelocityProfile::aroom(),
                 NavigationFlagsForceBallSmallObstacle);

        circleBall(m_attack, move_angle, 0, 0);
        Common::logDebug(":::{}", m_own_robot[m_attack].state().velocity.length());
        Common::logDebug("", m_own_robot[m_mid5].state().velocity.length());
        if (m_own_robot[m_attack].state().velocity.length() < 20 && m_own_robot[m_mid5].state().velocity.length() < 20)
        {
            m_func_count++;
        }
        if (m_func_count >= 10)
        {
            m_func_state        = 1;
            m_func_count        = 0;
            last_state_ball_pos = m_world_state.ball.position;
        }
    }
    else if (m_func_state == 1)
    {
        circleBall(m_attack, move_angle, 18, 0);
        waitForPass(m_mid5, false, &m_own_robot[m_attack].state().position);

        if (last_state_ball_pos.distanceTo(m_world_state.ball.position) > 400)
        { // TODO added this TEST IT with the kicker on (already tested without the kicker)
            m_func_count++;
        }
        if (m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position) > 400)
        {
            m_func_count++;
        }
        if (m_func_count >= 20)
        {
            m_func_state = 2;
            m_func_count = 0;
            temp_time.start();
        }
    }
    else if (m_func_state == 2)
    {
        waitForPass(m_mid5, false, &m_own_robot[m_attack].state().position);

        if (temp_time.time().seconds() > 2)
        {
            m_func_count++;
        }
        if (m_func_count >= 10)
        {
            m_func_state = 3;
            m_func_count = 0;
            temp_time.start();
        }
    }
    else if (m_func_state == 3)
    {
        if (std::fabs((m_own_robot[m_attack].target.angle - move_angle).deg()) > 90)
            std::swap(move_angle, temp_opp_ang);
        m_own_robot[m_attack].target.angle = move_angle;
        m_own_robot[m_mid5].target.angle    = temp_opp_ang;

        navigate(m_attack, m_world_state.ball.position.circleAroundPoint(temp_opp_ang, 250), VelocityProfile::aroom(),
                 NavigationFlagsForceBallSmallObstacle);

        navigate(m_mid5, m_world_state.ball.position.circleAroundPoint(move_angle, 250), VelocityProfile::aroom(),
                 NavigationFlagsForceBallSmallObstacle);

        if (m_world_state.ball.position.circleAroundPoint(temp_opp_ang, 250)
                    .distanceTo(m_own_robot[m_attack].state().position) < 100 &&
            m_world_state.ball.position.circleAroundPoint(move_angle, 250)
                    .distanceTo(m_own_robot[m_mid5].state().position) < 100 &&
            m_world_state.ball.velocity.length() < 10)
        {

            m_func_count++;
        }
        if (m_func_count >= 10)
        {
            m_func_state = 4;
            m_func_count = 0;
            t_pos        = m_world_state.ball.position;
        }
    }
    else if (m_func_state == 4)
    {
        if (std::fabs((m_own_robot[m_attack].target.angle - move_angle).deg()) > 90)
            std::swap(move_angle, temp_opp_ang);

        m_own_robot[m_attack].target.angle = move_angle;
        m_own_robot[m_mid5].target.angle    = temp_opp_ang;
        navigate(m_attack, t_pos.circleAroundPoint(temp_opp_ang, 75), VelocityProfile::sooski(),
                 NavigationFlagsForceNoObstacles);

        navigate(m_mid5, t_pos.circleAroundPoint(move_angle, 75), VelocityProfile::sooski(),
                 NavigationFlagsForceNoObstacles);

        bool ball_has_slipt = ((m_own_robot[m_attack].state().position + m_own_robot[m_mid5].state().position) / 2)
                                  .distanceTo(m_world_state.ball.position) > 300;

        if (m_world_state.ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            m_func_state = 3;
            m_func_count = 0;
        }

        if (t_pos.circleAroundPoint(temp_opp_ang, 75).distanceTo(m_own_robot[m_attack].state().position) < 40 &&
            t_pos.circleAroundPoint(move_angle, 75).distanceTo(m_own_robot[m_mid5].state().position) < 40)
        {
            m_func_count++;
        }
        if (m_func_count >= 10)
        {
            m_func_state = 5;
            m_func_count = 0;
            t_ang        = move_angle;
            t_opp_ang    = temp_opp_ang;
        }
    }
    else if (m_func_state == 5)
    {
        m_own_robot[m_attack].target.angle = t_ang;
        m_own_robot[m_mid5].target.angle    = t_opp_ang;

        navigate(m_attack, m_ref_state.designated_position.circleAroundPoint(t_opp_ang, 75), VelocityProfile::sooski(),
                 NavigationFlagsForceNoObstacles);

        navigate(m_mid5, m_ref_state.designated_position.circleAroundPoint(t_ang, 75), VelocityProfile::sooski(),
                 NavigationFlagsForceNoObstacles);

        bool ball_has_slipt = ((m_own_robot[m_attack].state().position + m_own_robot[m_mid5].state().position) / 2)
                                  .distanceTo(m_world_state.ball.position) > 300;

        if (m_world_state.ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            m_func_state = 3;
            m_func_count = 0;
        }

        if (m_ref_state.designated_position.circleAroundPoint(t_opp_ang, 75)
                    .distanceTo(m_own_robot[m_attack].state().position) < 40 &&
            m_ref_state.designated_position.circleAroundPoint(t_ang, 75).distanceTo(
                m_own_robot[m_mid5].state().position) < 40)
        {
            m_func_count++;
        }
        if (m_func_count >= 30)
        {
            m_func_state = 6;
            m_func_count = 0;
        }
    }
    else if (m_func_state == 6)
    {
        m_own_robot[m_attack].target.angle = t_ang;
        m_own_robot[m_mid5].target.angle    = t_opp_ang;
        Common::Vec2 target1               = m_ref_state.designated_position.circleAroundPoint(t_opp_ang, 550);
#if 0
        if (isOut(target1))
        {
            navigate(m_attack, Common::Vec2(), VelocityProfile::sooski());
        }
        else
#endif
        {
            navigate(m_attack, target1, VelocityProfile::sooski(), NavigationFlagsForceNoObstacles);
        }
        Common::Vec2 target2 = m_ref_state.designated_position.circleAroundPoint(t_ang, 550);

#if 0
        if (isOut(target1))
        {
            navigate(m_mid5, Common::Vec2(), VelocityProfile::sooski());
        }
        else
#endif
        {
            navigate(m_mid5, target2, VelocityProfile::sooski(), NavigationFlagsForceNoObstacles);
        }

        if (m_ref_state.designated_position.circleAroundPoint(t_opp_ang, 550)
                    .distanceTo(m_own_robot[m_attack].state().position) < 40 &&
            m_ref_state.designated_position.circleAroundPoint(t_ang, 550)
                    .distanceTo(m_own_robot[m_mid5].state().position) < 40)
        {
            m_func_count++;
        }
        if (m_func_count >= 30)
        {
            if (m_world_state.ball.position.distanceTo(m_ref_state.designated_position) > 95)
            {
                m_func_state = 3;
                m_func_count = 0;
            }
            else
            {
                m_func_state = 7;
                m_func_count = 0;
            }
        }
    }
    else
    {
        m_own_robot[m_attack].face(m_world_state.ball.position); // TODO test this
        navigate(m_attack, m_ref_state.designated_position.circleAroundPoint(t_opp_ang, 550), VelocityProfile::sooski(),
                 NavigationFlagsForceBallObstacle);
        m_own_robot[m_mid5].face(m_world_state.ball.position);
        navigate(m_mid5, m_ref_state.designated_position.circleAroundPoint(t_ang, 550), VelocityProfile::sooski(),
                 NavigationFlagsForceBallObstacle);

        bool success = m_ref_state.designated_position.distanceTo(m_world_state.ball.position) < 100.0;

        Common::logDebug("______IN___STATE_DONE_____");
        if (success)
        {
            Common::logInfo("MADE it!!!");
        }
        else
        {
            Common::logWarning("lost it!!!");
        }
    }
    Common::logInfo("______IN___STATE_{}_____", m_func_state);

    Common::logInfo("___DIS___{}", m_ref_state.designated_position.distanceTo(m_world_state.ball.position));
    Common::logInfo("__OUT__{}", isOut(m_world_state.ball.position));

    Common::debug().draw(m_ref_state.designated_position, Common::Color::red(), 20.0f);
}
} // namespace Tyr::Soccer
