#include "../ai.h"

namespace Tyr::Soccer
{
Common::Timer activeShootTimer;

void Ai::NormalPlayAtt(void)
{
    ManageAttRoles(); // sets the mid1, mid2, attack

    debugDraw = true;
    recievePass(dmf, Common::point_on_connecting_line(ball.Position, Common::vec2(side * field_width, 0), 2500));
    debugDraw = false;

    if (oneTouchType[attack] == allaf)
    {
        ERRTSetObstacles(attack, false, true);
        OwnRobot[attack].face(Common::vec2(-side * field_width, 0));
        // OwnRobot[robot_num].target.Angle=-90;
        ERRTNavigate2Point(attack, allafPos[attack], 0, 100, &VELOCITY_PROFILE_MAMOOLI);
        if (timer.time() > 2.5)
        {
            oneTouchType[attack] = oneTouch;
        }

        activeShootTimer.start();
    }

    else
    {
        float        ballReachTimeTmp = calculateBallRobotReachTime(attack, &VELOCITY_PROFILE_MAMOOLI) * 1.5;
        Common::vec2 ballReachPlace   = predictBallForwardAI(ballReachTimeTmp);
        float        ballGoalDot      = (Common::dot(Common::normalize(Common::vec2(ball.velocity.x, ball.velocity.y)),
                                                     Common::normalize(Common::vec2(-side * field_width, 0) - ballReachPlace)));
        if (0) // ballGoalDot > -0.6 && ballGoalDot < 0.7 && ball.velocity.magnitude > 900 )
        {
            Common::Angle passAngle = Common::to_angle(ball.velocity);
            tech_circle(attack, passAngle, 1, 0, 1, 0, 0, 1);
        }
        else
        {
            Common::vec2 openAngle = calculateOpenAngleToGoal(ball.Position, attack);

            bool mid1Reached = Common::length(OwnRobot[mid1].State.velocity) < 500;
            bool mid2Reached = Common::length(OwnRobot[mid2].State.velocity) < 500;

            bool mid1DisOk = Common::distance(OwnRobot[mid1].State.Position, ball.Position) > 2000;
            bool mid2DisOk = Common::distance(OwnRobot[mid2].State.Position, ball.Position) > 2000;

            bool mid1PassAngleOk =
                Common::dot(Common::normalize(OwnRobot[mid1].State.Position - ball.Position),
                            Common::normalize(Common::vec2(side * field_width, 0) - ball.Position)) < 0.75f;

            bool mid2PassAngleOk =
                Common::dot(Common::normalize(OwnRobot[mid2].State.Position - ball.Position),
                            Common::normalize(Common::vec2(side * field_width, 0) - ball.Position)) < 0.75f;

            LOG_DEBUG("pass angle dort mid1 : {}, mid2: {}",
                      Common::dot(Common::normalize(OwnRobot[mid1].State.Position - ball.Position),
                                  Common::normalize(Common::vec2(side * field_width, 0) - ball.Position)),
                      Common::dot(Common::normalize(OwnRobot[mid2].State.Position - ball.Position),
                                  Common::normalize(Common::vec2(side * field_width, 0) - ball.Position)));

            LOG_DEBUG("pass angle ok mid1 : {}, mid2: {}", mid1PassAngleOk, mid2PassAngleOk);

            bool mid1Seen = OwnRobot[mid1].State.seenState != Common::CompletelyOut;
            bool mid2Seen = OwnRobot[mid2].State.seenState != Common::CompletelyOut;

            bool mid1Suitable = mid1Seen && mid1Reached && mid1DisOk && mid1PassAngleOk;
            bool mid2Suitable = mid2Seen && mid2Reached && mid2DisOk && mid2PassAngleOk;

            if (mid1Suitable && mid2Suitable)
            {
                if (-side * OwnRobot[mid1].State.Position.x > -side * OwnRobot[mid2].State.Position.x)
                    mid2Suitable = false;
                else
                    mid1Suitable = false;
            }

            LOG_DEBUG("open angle: {}", openAngle.y);
            if (openAngle.y < 8 &&
                (findKickerOpp(-1, 150.0f) ==
                 -1)) //&& ( ball.Position.x * side < -2300 ) && ( fabs ( ball.Position.y ) > 1800 ) )
            {

                // float passAngle = Common::angle_with (
                // OwnRobot[randomParam<0.3?dmf:(randomParam<0.6?rmf:lmf)].State.Position , ball.Position );
                Common::Angle passAngle = Common::angle_with(
                    Common::vec2(-side * 1700, std::copysign(1700.0f, -ball.Position.y)), ball.Position);
                float chip_pow = 40;

                if (mid1Suitable)
                {
                    passAngle = Common::angle_with(OwnRobot[mid1].State.Position, ball.Position);
                    chip_pow  = 50.f * Common::distance(OwnRobot[mid1].State.Position, ball.Position) / 4000.0f;
                    chip_pow  = std::min(50.f, chip_pow);
                }
                else if (mid2Suitable)
                {
                    passAngle = Common::angle_with(OwnRobot[mid2].State.Position, ball.Position);
                    chip_pow  = 50.f * Common::distance(OwnRobot[mid2].State.Position, ball.Position) / 4000.0f;
                    chip_pow  = std::min(50.f, chip_pow);
                }
                else
                {
                    passAngle = Common::angle_with(Common::vec2(-side * field_width, 0), ball.Position);
                    chip_pow  = 0;
                }
                tech_circle(attack, passAngle, 0, chip_pow, 1, 0, 0, 1);
                // circle_ball(attack, passAngle, 0, chip_pow, 1);
            }
            else
            {

                Common::Angle shootAngle;
                shootAngle = Common::Angle::fromDeg(180.0f + openAngle.x);

                // ADDED by Dot_Blue:
                //                float shoot_pow;
                //                if(Common::distance(ball.Position,Common::vec2 ( -side*field_width , 0 )) < 2000) {
                //                    shoot_pow = 70 - OwnRobot[attack].State.velocity.magnitude * 0.01;
                //                } else {
                //                    shoot_pow = 50 - OwnRobot[attack].State.velocity.magnitude * 0.01;
                //                }

                //                float shoot_pow = 80 - OwnRobot[attack].State.velocity.magnitude * 0.01;
                float shoot_pow = 50 - Common::length(OwnRobot[attack].State.velocity) * 0.005;

                // if ( openAngle.y < 2 )
                //     shoot_pow = 0;
                if (Common::distance(OwnRobot[attack].State.Position, ball.Position) > 400)
                {
                    shoot_pow = 1;
                    activeShootTimer.start();
                }
                else if (goal_blocked(ball.Position, 200, 90))
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
                //	shootAngle = Common::angle_with(ball.Position, Common::vec2(side*field_width, 0));
                //	shoot_pow = 1;
                // }

                debugDraw = true;
                tech_circle(attack, shootAngle, shoot_pow, 0, 1, 0, 0, 0);
                // circle_ball(attack, 90, 80, 0, 1.0f);
                debugDraw = false;
            }
        }
    }

    if (ball.Position.y > 600)
    {
        recievePass(mid1, Common::vec2(-side * 250, 0));
    }
    else
    {
        recievePass(mid1, Common::vec2(-side * (field_width - 800), field_height - 800));
    }

    if (ball.Position.y < -600)
    {
        recievePass(mid2, Common::vec2(-side * 250, 0));
    }
    else
    {
        recievePass(mid2, Common::vec2(-side * (field_width - 800), -field_height + 800));
    }
}
} // namespace Tyr::Soccer
