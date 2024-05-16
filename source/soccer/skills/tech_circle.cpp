#include "../ai.h"

namespace Tyr::Soccer
{
int          lockAngleCounter = 0;
int          elendil          = 0;
Common::Vec2 Pelendil;
Common::Vec2 Ai::predictBallForwardAI(float timeAhead)
{
    Common::BallState _ball = m_world_state.ball;
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

float Ai::calculateRobotReachTime(int robot_num, Common::Vec2 dest, VelocityProfile vel_profile_type)
{
    const VelocityProfile vel_profile(vel_profile_type);
    return OwnRobot[robot_num].state().position.distanceTo(dest) / (vel_profile.max_spd * 42.5 * 0.5);
}

float Ai::calculateBallRobotReachTime(int robot_num, VelocityProfile vel_profile_type)
{
    static Common::MedianFilter<float, 5> predTFilt{};

    const float tMax  = 5.0;
    float       predT = tMax;
    for (float forwT = 0; forwT < tMax; forwT += 0.02f)
    {
        Common::Vec2 newBallPos  = predictBallForwardAI(forwT);
        float        robotReachT = calculateRobotReachTime(robot_num, newBallPos, vel_profile_type);
        if (robotReachT <= forwT)
        {
            predT = robotReachT;
            break;
        }
    }
    predTFilt.add(predT);

    return predTFilt.current();
}

void Ai::tech_circle(int robot_num, Common::Angle angle, int kick, int chip, bool needRRT, bool gameRestart, bool kiss,
                     bool dribbler, bool needOppRRT)
{
    // kick=100;
    if (gameRestart && (chip > 0))
    {
        chip_head = OwnRobot[robot_num].state().angle;
    }

    PredictedBall = m_world_state.ball.position;

    float d =
        OwnRobot[robot_num].state().position.distanceTo(m_world_state.ball.position.circleAroundPoint(angle, 200.0f));
    d += m_world_state.ball.position.distanceTo(m_world_state.ball.position.circleAroundPoint(angle, 200.0f));
    d -= 100;

    d /= 2000.0f;

    PredictedBall.x += d * m_world_state.ball.velocity.x / 45.0f;
    PredictedBall.y += d * m_world_state.ball.velocity.y / 45.0f;

    PredictedBall = predictBallForwardAI(d);

    Common::debug().draw(Common::Circle{PredictedBall, 50}, Common::Color::blue(), false);

    static bool passedBall = false;

    float prepPredictMul = 0.15;
    float goalPredictMul = 0.0;

    if (passedBall)
    {
        prepPredictMul = 0.0;
        goalPredictMul = -0.15;
    }

    Common::Vec2 ballToGoal = oppGoal() - m_world_state.ball.position;
    ballToGoal              = ballToGoal.normalized();
    float ballVelToGoalDot =
        (m_world_state.ball.velocity.x * ballToGoal.x + m_world_state.ball.velocity.y * ballToGoal.y);
    Common::Vec2 ballVelToGoal     = ballToGoal * ballVelToGoalDot;
    Common::Vec2 ballVelPrepToGoal = m_world_state.ball.velocity - ballVelToGoal;

    Common::debug().draw(
        Common::LineSegment{m_world_state.ball.position, m_world_state.ball.position + m_world_state.ball.velocity});
    Common::debug().draw(Common::LineSegment{m_world_state.ball.position, m_world_state.ball.position + ballVelToGoal},
                         Common::Color::pink());
    Common::debug().draw(
        Common::LineSegment{m_world_state.ball.position, m_world_state.ball.position + ballVelPrepToGoal},
        Common::Color::green());

    Common::Vec2 robotToBall    = OwnRobot[robot_num].state().position - m_world_state.ball.position;
    float        robotToBallDot = robotToBall.x * ballVelPrepToGoal.x + robotToBall.y * ballVelPrepToGoal.y;
    robotToBallDot /= robotToBall.length();
    robotToBallDot /= ballVelPrepToGoal.length();
    if (robotToBallDot > 0.3)
        passedBall = true;
    else if ((robotToBallDot < -0.2) ||
             (m_world_state.ball.position.distanceTo(OwnRobot[robot_num].state().position) > 1050))
        passedBall = false;

    if (passedBall)
    {
        Common::debug().draw(Common::Circle{OwnRobot[robot_num].state().position, 100}, Common::Color::gold(), false);
    }

    if (m_world_state.ball.position.distanceTo(OwnRobot[robot_num].state().position) < 400)
        circleReachedBehindBall = true;
    else if (m_world_state.ball.position.distanceTo(OwnRobot[robot_num].state().position) > 600)
        circleReachedBehindBall = false;

    Common::logDebug("circle dadam");

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

    if (dribbler)
    {
        if (OwnRobot[robot_num].state().position.distanceTo(m_world_state.ball.position) < 190)
            OwnRobot[robot_num].dribble(15);
    }

    if (gameRestart)
        OwnRobot[robot_num].face(PredictedBall);
    else
        OwnRobot[robot_num].target.angle = angle - Common::Angle::fromDeg(180.0f);

    Common::Angle hehe = PredictedBall.angleWith(OwnRobot[robot_num].state().position);
    hehe               = angle - hehe;

    if (needRRT)
        setObstacles(robot_num);
    else
        g_obs_map.resetMap();

    if ((std::fabs(hehe.deg()) < tetta)) //|| ( circleReachedBehindBall ) )
    {

        Common::debug().draw(Common::Circle{OwnRobot[robot_num].state().position, 100}, Common::Color::red(), false);
        if ((kick) || (chip))
        {
            if (circleReachedBehindBall)
            {
                Common::logDebug("reached");
                Common::Vec2 targetPoint;
                if (!gameRestart)
                {
                    targetPoint = PredictedBall.circleAroundPoint(
                        angle, std::min((r / 1.6f), std::fabs(hehe.deg()) * 320.0f / (tetta * 1.2f)));
                }
                else
                {
                    targetPoint =
                        PredictedBall.circleAroundPoint(angle, std::min(r, std::fabs(hehe.deg()) * 320.0f / (tetta)));
                }

                Common::logDebug("elendil: {}", elendil);
                Common::Angle hehe2 = PredictedBall.angleWith(OwnRobot[robot_num].state().position);
                hehe2               = angle - hehe2;
                bool el             = ((hehe2.deg() < 5) &&
                           (m_world_state.ball.position.distanceTo(OwnRobot[robot_num].state().position) < 100));
                if (el || (elendil > 0))
                {

                    targetPoint.x -= 150.0 * angle.cos();
                    targetPoint.y -= 150.0 * angle.sin();
                    targetPoint.x /= 1;
                    targetPoint.y /= 1;

                    Pelendil = targetPoint;

                    elendil--;
                    if (el)
                    {
                        elendil = 30;
                    }
                    navigate(robot_num, Pelendil, VelocityProfile::kharaki());
                }
                else
                {
                    targetPoint = (targetPoint * 3.0f) - OwnRobot[robot_num].state().position;
                    targetPoint /= 2.0f;
                    navigate(robot_num, targetPoint, VelocityProfile::kharaki());
                }
            }
            else
                navigate(robot_num,
                         PredictedBall.circleAroundPoint(angle, std::min(r, std::fabs(hehe.deg()) * 320.0f / tetta)),
                         VelocityProfile::mamooli());
        }
        else
            navigate(robot_num, PredictedBall.circleAroundPoint(angle + hehe * 0.0f, r));
    }
    else
    {
        Common::debug().draw(Common::Circle{OwnRobot[robot_num].state().position, 100}, Common::Color::orange(), false);

        hehe = PredictedBall.angleWith(OwnRobot[robot_num].state().position) +
               Common::Angle::fromDeg(Common::sign(hehe.deg()) * tetta);
        if (passedBall)
            navigate(robot_num, PredictedBall.circleAroundPoint(hehe, r));
        else
            navigate(robot_num, PredictedBall.circleAroundPoint(hehe, r), VelocityProfile::kharaki());
    }

    Common::debug().draw(Common::Circle{OwnRobot[robot_num].state().position, 100}, Common::Color::violet(), false);
    Common::debug().draw(
        Common::LineSegment{
            OwnRobot[robot_num].state().position,
            OwnRobot[robot_num].state().position +
                (OwnRobot[robot_num].target.position - OwnRobot[robot_num].state().position).normalized() * 1000.0f},
        Common::Color::black());

    if ((kick > 0) || (chip > 0))
    {
        Common::Vec2  tmpPos = OwnRobot[robot_num].state().position;
        Common::Angle tmpAng = OwnRobot[robot_num].state().angle;

        if (std::fabs((OwnRobot[robot_num].target.angle - OwnRobot[robot_num].state().angle).deg()) < 40)
            lockAngleCounter++;
        else
            lockAngleCounter = 0;

        // if ( lockAngleCounter > 1 )
        {
            // if ( chip )
            OwnRobot[robot_num].chip(chip);
            // else
            kick            = std::min(80, kick);
            float vel_delta = OwnRobot[robot_num].state().velocity.length() / 100.0f;
            vel_delta       = std::min(40.0f, vel_delta);
            vel_delta       = kick - vel_delta;
            OwnRobot[robot_num].shoot(kick);
        }
    }
}
} // namespace Tyr::Soccer
