#include "../ai.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer;

void Ai::NormalPlayAtt()
{
    ManageAttRoles(); // sets the mid1, mid2, attack

    receivePass(dmf, Common::worldState().ball.position.pointOnConnectingLine(Common::Vec2(side * Common::field().width, 0), 2500));

    if (oneTouchType[attack] == allaf)
    {
        ERRTSetObstacles(attack, false, true);
        OwnRobot[attack].face(Common::Vec2(-side * Common::field().width, 0));
        // OwnRobot[robot_num].target.angle=-90;
        ERRTNavigate2Point(attack, allafPos[attack], 100, &VELOCITY_PROFILE_MAMOOLI);
        if (timer.time() > 2.5)
        {
            oneTouchType[attack] = oneTouch;
        }

        activeShootTimer.start();
    }

    else
    {
        float        ballReachTimeTmp = calculateBallRobotReachTime(attack, &VELOCITY_PROFILE_MAMOOLI) * 1.5;
        Common::Vec2 ballReachPlace   = predictBallForwardAI(ballReachTimeTmp);
        float        ballGoalDot =
            Common::worldState().ball.velocity.normalized().dot((Common::Vec2(-side * Common::field().width, 0) - ballReachPlace).normalized());
        if (0) // ballGoalDot > -0.6 && ballGoalDot < 0.7 && Common::worldState().ball.velocity.length > 900 )
        {
            Common::Angle passAngle = Common::worldState().ball.velocity.toAngle();
            tech_circle(attack, passAngle, 1, 0, 1, 0, 0, 1);
        }
        else
        {
            OpenAngle openAngle = calculateOpenAngleToGoal(Common::worldState().ball.position, attack);

            bool mid1Reached = OwnRobot[mid1].state().velocity.length() < 500;
            bool mid2Reached = OwnRobot[mid2].state().velocity.length() < 500;

            bool mid1DisOk = OwnRobot[mid1].state().position.distanceTo(Common::worldState().ball.position) > 2000;
            bool mid2DisOk = OwnRobot[mid2].state().position.distanceTo(Common::worldState().ball.position) > 2000;

            bool mid1PassAngleOk = (OwnRobot[mid1].state().position - Common::worldState().ball.position)
                                       .normalized()
                                       .dot((Common::Vec2(side * Common::field().width, 0) - Common::worldState().ball.position).normalized()) < 0.75f;

            bool mid2PassAngleOk = (OwnRobot[mid2].state().position - Common::worldState().ball.position)
                                       .normalized()
                                       .dot((Common::Vec2(side * Common::field().width, 0) - Common::worldState().ball.position).normalized()) < 0.75f;

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
                (findKickerOpp(-1, 150.0f) ==
                 -1)) //&& ( Common::worldState().ball.position.x * side < -2300 ) && ( std::fabs ( Common::worldState().ball.position.y ) > 1800 ) )
            {

                // float passAngle = Common::Vec2::angleWith (
                // OwnRobot[randomParam<0.3?dmf:(randomParam<0.6?rmf:lmf)].state().position , Common::worldState().ball.position );
                Common::Angle passAngle =
                    Common::Vec2(-side * 1700, Common::sign(-Common::worldState().ball.position.y) * 1700.0f).angleWith(Common::worldState().ball.position);
                float chip_pow = 40;

                if (mid1Suitable)
                {
                    passAngle = OwnRobot[mid1].state().position.angleWith(Common::worldState().ball.position);
                    chip_pow  = 50.f * OwnRobot[mid1].state().position.distanceTo(Common::worldState().ball.position) / 4000.0f;
                    chip_pow  = std::min(50.f, chip_pow);
                }
                else if (mid2Suitable)
                {
                    passAngle = OwnRobot[mid2].state().position.angleWith(Common::worldState().ball.position);
                    chip_pow  = 50.f * OwnRobot[mid2].state().position.distanceTo(Common::worldState().ball.position) / 4000.0f;
                    chip_pow  = std::min(50.f, chip_pow);
                }
                else
                {
                    passAngle = Common::Vec2(-side * Common::field().width, 0).angleWith(Common::worldState().ball.position);
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
                //                if(Common::Vec2::distance(Common::worldState().ball.position,Common::Vec2 ( -side*Common::field().width , 0 )) <
                //                2000) {
                //                    shoot_pow = 70 - OwnRobot[attack].state().velocity.length * 0.01;
                //                } else {
                //                    shoot_pow = 50 - OwnRobot[attack].state().velocity.length * 0.01;
                //                }

                //                float shoot_pow = 80 - OwnRobot[attack].state().velocity.length * 0.01;
                float shoot_pow = 50 - OwnRobot[attack].state().velocity.length() * 0.005;

                // if ( openAngle.y < 2 )
                //     shoot_pow = 0;
                if (OwnRobot[attack].state().position.distanceTo(Common::worldState().ball.position) > 400)
                {
                    shoot_pow = 1;
                    activeShootTimer.start();
                }
                else if (goalBlocked(Common::worldState().ball.position, 200, 90))
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
                //	shootAngle = Common::Vec2::angleWith(Common::worldState().ball.position, Common::Vec2(side*Common::field().width, 0));
                //	shoot_pow = 1;
                // }

                tech_circle(attack, shootAngle, shoot_pow, 0, 1, 0, 0, 0);
                // circle_ball(attack, 90, 80, 0, 1.0f);
            }
        }
    }

    if (Common::worldState().ball.position.y > 600)
    {
        receivePass(mid1, Common::Vec2(-side * 250, 0));
    }
    else
    {
        receivePass(mid1, Common::Vec2(-side * (Common::field().width - 800), Common::field().height - 800));
    }

    if (Common::worldState().ball.position.y < -600)
    {
        receivePass(mid2, Common::Vec2(-side * 250, 0));
    }
    else
    {
        receivePass(mid2, Common::Vec2(-side * (Common::field().width - 800), -Common::field().height + 800));
    }
}
} // namespace Tyr::Soccer
