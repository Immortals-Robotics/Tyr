#include "../ai.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer;

void Ai::NormalPlayAtt()
{
    Common::Vec2 oppgoal_p1 = Common::Vec2(-side * Common::field().width, -side * Common::field().goal_width / -2);
    Common::Vec2 oppgoal_p2 = Common::Vec2(-side * Common::field().width, -side * Common::field().goal_width / 2);
    Common::debug().draw(Common::Triangle{oppgoal_p1, m_world_state.ball.position, oppgoal_p2},
                         Common::Color::red().transparent(), true);

    ManageAttRoles(); // sets the mid1, mid2, attack

    receivePass(dmf,
                m_world_state.ball.position.pointOnConnectingLine(Common::Vec2(side * Common::field().width, 0), 2500));

    if (oneTouchType[attack] == allaf)
    {
        setObstacles(attack);
        OwnRobot[attack].face(Common::Vec2(-side * Common::field().width, 0));
        // OwnRobot[robot_num].target.angle=-90;
        navigate(attack, allafPos[attack], VelocityProfile::mamooli());
        if (timer.time() > 2.5)
        {
            oneTouchType[attack] = oneTouch;
        }

        activeShootTimer.start();
    }

    else
    {
        float        ballReachTimeTmp = calculateBallRobotReachTime(attack, VelocityProfile::mamooli()) * 1.5;
        Common::Vec2 ballReachPlace   = predictBallForwardAI(ballReachTimeTmp);
        float        ballGoalDot      = m_world_state.ball.velocity.normalized().dot(
            (Common::Vec2(-side * Common::field().width, 0) - ballReachPlace).normalized());
        if (0) // ballGoalDot > -0.6 && ballGoalDot < 0.7 && m_world_state.ball.velocity.length > 900 )
        {
            Common::Angle passAngle = m_world_state.ball.velocity.toAngle();
            tech_circle(attack, passAngle, 1, 0, 1, 0, 0, 1);
        }
        else
        {
            OpenAngle openAngle = calculateOpenAngleToGoal(m_world_state.ball.position, attack);

            bool mid1Reached = OwnRobot[mid1].state().velocity.length() < 500;
            bool mid2Reached = OwnRobot[mid2].state().velocity.length() < 500;

            bool mid1DisOk = OwnRobot[mid1].state().position.distanceTo(m_world_state.ball.position) > 2000;
            bool mid2DisOk = OwnRobot[mid2].state().position.distanceTo(m_world_state.ball.position) > 2000;

            bool mid1PassAngleOk =
                (OwnRobot[mid1].state().position - m_world_state.ball.position)
                    .normalized()
                    .dot((Common::Vec2(side * Common::field().width, 0) - m_world_state.ball.position).normalized()) <
                0.75f;

            bool mid2PassAngleOk =
                (OwnRobot[mid2].state().position - m_world_state.ball.position)
                    .normalized()
                    .dot((Common::Vec2(side * Common::field().width, 0) - m_world_state.ball.position).normalized()) <
                0.75f;

            Common::logDebug("pass angle ok mid1 : {}, mid2: {}", mid1PassAngleOk, mid2PassAngleOk);

            bool mid1Seen = OwnRobot[mid1].state().seen_state != Common::SeenState::CompletelyOut;
            bool mid2Seen = OwnRobot[mid2].state().seen_state != Common::SeenState::CompletelyOut;

            bool mid1Suitable = mid1Seen && mid1Reached && mid1DisOk && mid1PassAngleOk;
            bool mid2Suitable = mid2Seen && mid2Reached && mid2DisOk && mid2PassAngleOk;

            if (mid1Suitable && mid2Suitable)
            {
                if (-side * OwnRobot[mid1].state().position.x > -side * OwnRobot[mid2].state().position.x)
                    mid2Suitable = false;
                else
                    mid1Suitable = false;
            }

            Common::logDebug("open angle: {}", openAngle.magnitude.deg());
            if (openAngle.magnitude.deg() < 8 &&
                (findKickerOpp(-1, 150.0f) == -1)) //&& ( m_world_state.ball.position.x * side < -2300 ) && ( std::fabs
                                                   //( m_world_state.ball.position.y ) > 1800 ) )
            {

                // float passAngle = Common::Vec2::angleWith (
                // OwnRobot[randomParam<0.3?dmf:(randomParam<0.6?rmf:lmf)].state().position ,
                // m_world_state.ball.position );
                Common::Angle passAngle =
                    Common::Vec2(-side * 1700, Common::sign(-m_world_state.ball.position.y) * 1700.0f)
                        .angleWith(m_world_state.ball.position);
                float chip_pow = 40;

                if (mid1Suitable)
                {
                    passAngle = OwnRobot[mid1].state().position.angleWith(m_world_state.ball.position);
                    chip_pow = 50.f * OwnRobot[mid1].state().position.distanceTo(m_world_state.ball.position) / 4000.0f;
                    chip_pow = std::min(50.f, chip_pow);
                }
                else if (mid2Suitable)
                {
                    passAngle = OwnRobot[mid2].state().position.angleWith(m_world_state.ball.position);
                    chip_pow = 50.f * OwnRobot[mid2].state().position.distanceTo(m_world_state.ball.position) / 4000.0f;
                    chip_pow = std::min(50.f, chip_pow);
                }
                else
                {
                    passAngle = Common::Vec2(-side * Common::field().width, 0).angleWith(m_world_state.ball.position);
                    chip_pow  = 0;
                }
                tech_circle(attack, passAngle, 0, chip_pow, 1, 0, 0, 1);
                // circle_ball(attack, passAngle, 0, chip_pow, 1);
            }
            else
            {

                Common::Angle shootAngle;
                shootAngle = Common::Angle::fromDeg(180.0f) + openAngle.center;

                // ADDED by Dot_Blue:
                //                float shoot_pow;
                //                if(Common::Vec2::distance(m_world_state.ball.position,Common::Vec2 (
                //                -side*Common::field().width , 0 )) < 2000) {
                //                    shoot_pow = 70 - OwnRobot[attack].state().velocity.length * 0.01;
                //                } else {
                //                    shoot_pow = 50 - OwnRobot[attack].state().velocity.length * 0.01;
                //                }

                //                float shoot_pow = 80 - OwnRobot[attack].state().velocity.length * 0.01;
                float shoot_pow = 50 - OwnRobot[attack].state().velocity.length() * 0.005;

                // if ( openAngle.y < 2 )
                //     shoot_pow = 0;
                if (OwnRobot[attack].state().position.distanceTo(m_world_state.ball.position) > 400)
                {
                    shoot_pow = 1;
                    activeShootTimer.start();
                }
                else if (goalBlocked(m_world_state.ball.position, 200, 90))
                {
                    shoot_pow = 1;
                }
                else
                {
                    // if ( activeShootTimer.time()<0.3 )
                    //{
                    //     shoot_pow = 1;
                    // }
                }

                // if (attackFuckingAngle()) {
                //	shootAngle = Common::Vec2::angleWith(m_world_state.ball.position,
                // Common::Vec2(side*Common::field().width, 0)); 	shoot_pow = 1;
                // }

                tech_circle(attack, shootAngle, shoot_pow, 0, 1, 0, 0, 0);
                // circle_ball(attack, 90, 80, 0, 1.0f);
            }
        }
    }

    if (m_world_state.ball.position.y > 600)
    {
        receivePass(mid1, Common::Vec2(-side * 250, 0));
    }
    else
    {
        receivePass(mid1, Common::Vec2(-side * (Common::field().width - 800), Common::field().height - 800));
    }

    if (m_world_state.ball.position.y < -600)
    {
        receivePass(mid2, Common::Vec2(-side * 250, 0));
    }
    else
    {
        receivePass(mid2, Common::Vec2(-side * (Common::field().width - 800), -Common::field().height + 800));
    }
}
} // namespace Tyr::Soccer
