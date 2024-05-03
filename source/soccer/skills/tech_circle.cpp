#include "../ai.h"

namespace Tyr::Soccer
{
int          lockAngleCounter = 0;
int          elendil          = 0;
Common::Vec2 Pelendil;
Common::Vec2 Ai::predictBallForwardAI(float timeAhead)
{
    Common::BallState _ball = Common::worldState().ball;
    _ball.position.x /= 1000.0f;
    _ball.position.y /= 1000.0f;
    _ball.velocity.x /= 1000.0f;
    _ball.velocity.y /= 1000.0f;
    float k = 0.25f; // velocity derate every sec(units (m/s)/s)
    // float frame_rate = 61.0f;
    // float tsample = (float)1.0f/(float)frame_rate;

    float vx_vision = _ball.velocity.x;
    float vy_vision = _ball.velocity.y;

    float xpos_vision = _ball.position.x;
    float ypos_vision = _ball.position.y;

    float vball_vision = float(sqrt(vx_vision * vx_vision + vy_vision * vy_vision));

    float t = timeAhead;

    float v     = vball_vision - k * t;
    float dist0 = vball_vision * t - k * (t * t) / 2.0f;

    float dist;
    float vball_pred;

    // if speed turns out to be negative..it means that ball has stopped, so calculate that amount of
    // distance traveled
    if (v < 0)
    {
        vball_pred = 0.0f;
        dist       = (vball_vision * vball_vision) * k / 2.0f;
        // i.e the ball has stopped, so take a newer vision data for the prediction
    }
    else
    {
        vball_pred = v;
        dist       = dist0;
    }

    if (vball_vision != 0)
    {
        _ball.velocity.x = vball_pred * (vx_vision) / vball_vision;
        _ball.velocity.y = vball_pred * (vy_vision) / vball_vision;
        _ball.position.x = (xpos_vision + dist * (vx_vision) / vball_vision);
        _ball.position.y = (ypos_vision + dist * (vy_vision) / vball_vision);
    }

    _ball.velocity.x *= (float) 1000.0;
    _ball.velocity.y *= (float) 1000.0;
    _ball.position.x *= (float) 1000.0;
    _ball.position.y *= (float) 1000.0;

    return _ball.position;
}

float Ai::calculateRobotReachTime(int robot_num, Common::Vec2 dest, VelocityProfile *vel_profile)
{
    return OwnRobot[robot_num].State.position.distanceTo(dest) / (vel_profile->max_spd.length() * 42.5 * 0.5);
}

float Ai::calculateBallRobotReachTime(int robot_num, VelocityProfile *vel_profile)
{
    static Common::MedianFilter<float> predTFilt(5);

    const float tMax  = 5.0;
    float       predT = tMax;
    for (float forwT = 0; forwT < tMax; forwT += 0.02f)
    {
        Common::Vec2 newBallPos  = predictBallForwardAI(forwT);
        float        robotReachT = calculateRobotReachTime(robot_num, newBallPos, vel_profile);
        if (robotReachT <= forwT)
        {
            predT = robotReachT;
            break;
        }
    }
    predTFilt.AddData(predT);

    return predTFilt.GetCurrent();
}

void Ai::tech_circle(int robot_num, Common::Angle angle, int kick, int chip, bool needRRT, bool gameRestart, bool kiss,
                     bool dribbler, bool needOppRRT)
{
    // kick=100;
    if (gameRestart && (chip > 0))
    {
        chip_head = OwnRobot[robot_num].State.angle;
    }

    if (1) // ( std::fabs(NormalizeAngle(lastBallDirection-Common::worldState().ball.velocity.direction) ) > 5 ) || (
           // std::fabs(lastBallMagnitude-Common::worldState().ball.velocity.length) > 80 ) || ( Common::worldState().ball.velocity.length < 100 ) || (
           // Common::Vec2::distance(Common::worldState().ball.position, PredictedBall) < 150) )
    {
        PredictedBall = Common::worldState().ball.position;

        // PredictedBall = predictBallForwardAI(calculateBallRobotReachTime(robot_num,&VELOCITY_PROFILE_MAMOOLI));
        if (1) // ( std::fabs(NormalizeAngle(lastBallDirection-Common::worldState().ball.velocity.direction) ) > 5 ) || (
               // std::fabs(lastBallMagnitude-Common::worldState().ball.velocity.length) > 80 ) || ( Common::worldState().ball.velocity.length < 100 ) || (
               // Common::Vec2::distance(Common::worldState().ball.position, PredictedBall) < 150) )
        {
            float d = OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position.circleAroundPoint(angle, 200.0f));
            d += Common::worldState().ball.position.distanceTo(Common::worldState().ball.position.circleAroundPoint(angle, 200.0f));
            d -= 100;

            // float d = Common::Vec2::distance ( OwnRobot[robot_num].State.position , Common::worldState().ball.position );

            d /= 2000.0f;
            // d = 0.0f;

            // d *= 50.0f;

            // d = std::min ( 20.0f , d );

            // d += 5.0f;

            // d = 0.0f;

            PredictedBall.x += d * Common::worldState().ball.velocity.x / 45.0f;
            PredictedBall.y += d * Common::worldState().ball.velocity.y / 45.0f;

            PredictedBall = predictBallForwardAI(d);
        }

        Common::debug().drawCircle(PredictedBall, 100, "", Common::Blue);

        /*float d = Common::Vec2::distance ( OwnRobot[robot_num].State.position , Common::circleAroundPoint (
        Common::worldState().ball.position , angle , 200.0f ) ); d += Common::Vec2::distance ( Common::worldState().ball.position , Common::circleAroundPoint (
        Common::worldState().ball.position , angle , 200.0f ) ); d -= 100;

        //float d = Common::Vec2::distance ( OwnRobot[robot_num].State.position , Common::worldState().ball.position );

        d /= 2000.0f;
        //d = 0.0f;

        //d *= 50.0f;

        //d = std::min ( 20.0f , d );

        //d += 5.0f;

        //d = 0.0f;

        PredictedBall.x += d * Common::worldState().ball.velocity.x / 45.0f;
        PredictedBall.y += d * Common::worldState().ball.velocity.y / 45.0f;

        PredictedBall = predictBallForwardAI(d);*/
    }

    static bool passedBall = false;

    float prepPredictMul = 0.15;
    float goalPredictMul = 0.0;

    if (passedBall)
    {
        prepPredictMul = 0.0;
        goalPredictMul = -0.15;
    }

    Common::Vec2 ballToGoal        = Common::Vec2(-side * Common::worldState().field.width, 0) - Common::worldState().ball.position;
    ballToGoal                     = ballToGoal.normalized();
    float        ballVelToGoalDot  = (Common::worldState().ball.velocity.x * ballToGoal.x + Common::worldState().ball.velocity.y * ballToGoal.y);
    Common::Vec2 ballVelToGoal     = Common::Vec2(ballToGoal.x * ballVelToGoalDot, ballToGoal.y * ballVelToGoalDot);
    Common::Vec2 ballVelPrepToGoal = Common::Vec2(Common::worldState().ball.velocity.x - ballVelToGoal.x, Common::worldState().ball.velocity.y - ballVelToGoal.y);

    // Common::debug().drawLineSegment(Common::worldState().ball.position,Common::worldState().ball.position+Common::Vec2(Common::worldState().ball.velocity.x, Common::worldState().ball.velocity.y)/1.0f);
    // Common::debug().drawLineSegment(Common::worldState().ball.position,Common::worldState().ball.position+ballVelToGoal/1.0f , Pink);
    // Common::debug().drawLineSegment(Common::worldState().ball.position,Common::worldState().ball.position+ballVelPrepToGoal/1.0f , Green);

    // PredictedBall += ballVelPrepToGoal * prepPredictMul;
    // PredictedBall += ballVelToGoal * goalPredictMul;

    // Common::debug().drawCircle(PredictedBall, 70, Purple);

    Common::Vec2 robotToBall    = OwnRobot[robot_num].State.position - Common::worldState().ball.position;
    float        robotToBallDot = robotToBall.x * ballVelPrepToGoal.x + robotToBall.y * ballVelPrepToGoal.y;
    robotToBallDot /= robotToBall.length();
    robotToBallDot /= ballVelPrepToGoal.length();
    // if ( !passedBall )
    //{
    // if ( Common::Vec2::distance(PredictedBall, OwnRobot[robot_num].State.position) < 250 )
    if (robotToBallDot > 0.3)
        passedBall = true;
    //}
    // else if ( Common::Vec2::distance(Common::worldState().ball.position, OwnRobot[robot_num].State.position) > 750 )
    else if ((robotToBallDot < -0.2) || (Common::worldState().ball.position.distanceTo(OwnRobot[robot_num].State.position) > 1050))
        passedBall = false;

    if (passedBall)
    {
        Common::debug().drawCircle(OwnRobot[robot_num].State.position, 200, "", Common::Gold);
    }

    if (Common::worldState().ball.position.distanceTo(OwnRobot[robot_num].State.position) < 400)
        circleReachedBehindBall = true;
    else if (Common::worldState().ball.position.distanceTo(OwnRobot[robot_num].State.position) > 600)
        circleReachedBehindBall = false;

    if (0)
    {
        ERRTSetObstacles(robot_num, 0, 0);
        planner[robot_num].init(Common::worldState().ball.position, Common::Vec2(-0, 0), 9);
        Common::Vec2 wayp = planner[robot_num].plan();

        // wayp = Common::Vec2 ( -3025 , 0 );

        angle = wayp.angleWith(Common::worldState().ball.position);
    }

    // std::cout << "circle dadam";

    float r = 150.0f;

    if (kiss)
    {
        r = 82.0f;
    }

    float tetta = 10.0f;

    if ((kick > 0) || (chip > 0))
    {
        if (gameRestart)
        {
            r     = 200.0f;
            tetta = 20.0f;
        }
        else
        {
            r     = 400.0f;
            tetta = 32.0f;
            // if ( !passedBall )
            //	r *= 1.5;
        }
    }

    // dribbler=0;
    if (dribbler)
    {
        if (OwnRobot[robot_num].State.position.distanceTo(Common::worldState().ball.position) < 190)
            OwnRobot[robot_num].Dribble(15);
    }

    if (gameRestart)
        OwnRobot[robot_num].face(PredictedBall);
    else
        OwnRobot[robot_num].target.angle = angle - Common::Angle::fromDeg(180.0f);

    Common::Angle hehe = PredictedBall.angleWith(OwnRobot[robot_num].State.position);
    hehe               = angle - hehe;

    if (needRRT)
    {
        ERRTSetObstacles(robot_num, false, true);
    }

    else
        obs_map.resetMap();

    if ((std::fabs(hehe.deg()) < tetta)) //|| ( circleReachedBehindBall ) )
    {

        Common::debug().drawCircle(Common::Vec2(0, 0), 1000, "", Common::Red);
        // hehe = angle;
        // if ( OwnRobot[2].State.angle < 0 )
        if ((kick) || (chip))
        {

            if (circleReachedBehindBall)
            {
                // std::cout << "								reached	";
                Common::Vec2 targetPoint;
                if (!gameRestart)
                {
                    targetPoint = PredictedBall.circleAroundPoint(
                        angle, std::min((r / 1.6f), std::fabs(hehe.deg()) * 320.0f / (tetta * 1.2f)));
                }
                else
                    targetPoint =
                        PredictedBall.circleAroundPoint(angle, std::min(r, std::fabs(hehe.deg()) * 320.0f / (tetta)));

                // Common::Line newLine = Common::Line::fromTwoPoints ( Common::Vec2 ( targetPoint.x , targetPoint.y ) ,
                // Common::Vec2 ( OwnRobot[robot_num].State.position.x , OwnRobot[robot_num].State.position.y ) ); Circle
                // newCircle ( Common::Vec2 ( OwnRobot[robot_num].State.position.x , OwnRobot[robot_num].State.position.y
                // ) , Common::Vec2::distance(targetPoint, OwnRobot.State.position) * 2 );
                if (1) //! gameRestart )
                {
                    // std::cout << "elendil: " << elendil;
                    Common::Angle hehe2 = PredictedBall.angleWith(OwnRobot[robot_num].State.position);
                    hehe2               = angle - hehe2;
                    bool el =
                        ((hehe2.deg() < 5) && (Common::worldState().ball.position.distanceTo(OwnRobot[robot_num].State.position) < 100));
                    if (el || (elendil > 0))
                    {

                        targetPoint.x -= 150.0 * angle.cos();
                        targetPoint.y -= 150.0 * angle.sin();
                        // targetPoint = Common::Vec2(3*targetPoint.x-2*OwnRobot[robot_num].State.position.x,
                        // 3*targetPoint.y-2*OwnRobot[robot_num].State.position.y);
                        targetPoint.x /= 1;
                        targetPoint.y /= 1;

                        // if (elendil<=0)
                        {
                            Pelendil = targetPoint;
                        }

                        elendil--;
                        if (el)
                        {
                            elendil = 30;
                        }
                        ERRTNavigate2Point(robot_num, Pelendil, 0, 100, &VELOCITY_PROFILE_KHARAKI);
                    }
                    else
                    {
                        // targetPoint.x -= 550.0 * pow(std::fabs ( sinDeg(hehe) ),4.0) * cosDeg(angle);
                        // targetPoint.y -= 550.0 * pow(std::fabs ( sinDeg(hehe) ),4.0) * sinDeg(angle);
                        targetPoint = Common::Vec2(3 * targetPoint.x - OwnRobot[robot_num].State.position.x,
                                                   3 * targetPoint.y - OwnRobot[robot_num].State.position.y);
                        targetPoint.x /= 2;
                        targetPoint.y /= 2;
                        ERRTNavigate2Point(robot_num, targetPoint, 0, 100, &VELOCITY_PROFILE_KHARAKI);
                    }
                    // Halt(robot_num);
                }

                else
                {
                    ERRTNavigate2Point(robot_num, targetPoint, 0, 100, &VELOCITY_PROFILE_MAMOOLI);
                }

                // targetPoint = Common::Vec2((targetPoint.x+OwnRobot[robot_num].State.position.x)/2,
                // (targetPoint.y+OwnRobot[robot_num].State.position.y)/2);
            }
            else
                ERRTNavigate2Point(
                    robot_num,
                    PredictedBall.circleAroundPoint(angle, std::min(r, std::fabs(hehe.deg()) * 320.0f / tetta)), 0, 100,
                    &VELOCITY_PROFILE_MAMOOLI);
            // circleReachedBehindBall = true;
        }
        else
            ERRTNavigate2Point(robot_num, PredictedBall.circleAroundPoint(angle + hehe * 0.0f, r), 0, 100);
    }
    else
    {
        Common::debug().drawCircle(Common::Vec2(0, 0), 1000, "", Common::Orange);

        hehe = PredictedBall.angleWith(OwnRobot[robot_num].State.position) +
               Common::Angle::fromDeg(Common::sign(hehe.deg()) * tetta);
        // if ( Common::Vec2::distance ( OwnRobot[robot_num].State.position , Common::worldState().ball.position ) < 200 )
        //	ERRTNavigate2Point ( robot_num , Common::circleAroundPoint ( PredictedBall , hehe , r ) , 0 , 20 );
        // else
        if (passedBall)
            ERRTNavigate2Point(robot_num, PredictedBall.circleAroundPoint(hehe, r), 0, 100);
        else
            ERRTNavigate2Point(robot_num, PredictedBall.circleAroundPoint(hehe, r), 0, 100, &VELOCITY_PROFILE_KHARAKI);
    }

    Common::debug().drawCircle(OwnRobot[robot_num].State.position, 90, "", Common::Blue_Violet);
    Common::debug().drawLineSegment(
        OwnRobot[robot_num].State.position,
        OwnRobot[robot_num].State.position +
            (OwnRobot[robot_num].target.position - OwnRobot[robot_num].State.position).normalized() * 1000.0f,
        "", Common::Black);

    if ((kick > 0) || (chip > 0))
    {
        Common::Vec2  tmpPos = OwnRobot[robot_num].State.position;
        Common::Angle tmpAng = OwnRobot[robot_num].State.angle;
        /*if ( side == -1 )
        {
            tmpPos.x = -tmpPos.x;
            tmpAng += 180.0f;
        }*/

        if (std::fabs((OwnRobot[robot_num].target.angle - OwnRobot[robot_num].State.angle).deg()) < 40)
            lockAngleCounter++;
        else
            lockAngleCounter = 0;

        // if ( lockAngleCounter > 1 )
        {
            // if ( chip )
            OwnRobot[robot_num].Chip(chip);
            // else
            kick            = std::min(80, kick);
            float vel_delta = OwnRobot[robot_num].State.velocity.length() / 100.0f;
            vel_delta       = std::min(40.0f, vel_delta);
            vel_delta       = kick - vel_delta;
            OwnRobot[robot_num].Shoot(kick);
        }
    }
}
} // namespace Tyr::Soccer
