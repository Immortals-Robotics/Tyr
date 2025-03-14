#include "../ai.h"

namespace Tyr::Soccer
{
int lockAngleCounter = 0;
int elendil          = 0;

Common::Vec2 Ai::predictBallForwardAINew(const float t_time_ahead)
{
    Common::WorldState world_state = m_world_state;
    Common::Vec3       pos_3d      = Common::Vec3(world_state.ball.position.x, world_state.ball.position.y, 0.0);
    Common::Vec3       vel_3d      = Common::Vec3(world_state.ball.velocity.x, world_state.ball.velocity.y, 0.0);

    m_ball_predictor->process(pos_3d, vel_3d, true, world_state.own_robot, world_state.opp_robot);
    auto filtered_position =
        m_ball_predictor->getFutureState(t_time_ahead, world_state.own_robot, world_state.opp_robot,
                                         950. * Common::field().ball_model_straight.acc_roll);
    return filtered_position.xy();
}

Common::Vec2 Ai::predictBallForwardAI(const float t_time_ahead)
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

    float vball_vision = static_cast<float>(sqrt(vx_vision * vx_vision + vy_vision * vy_vision));

    float t = t_time_ahead;

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

    _ball.velocity.x *= static_cast<float>(1000.0);
    _ball.velocity.y *= static_cast<float>(1000.0);
    _ball.position.x *= static_cast<float>(1000.0);
    _ball.position.y *= static_cast<float>(1000.0);

    return _ball.position;
}

float Ai::calculateRobotReachTime(const int t_robot_num, const Common::Vec2 t_dest, const VelocityProfile t_profile)
{
    const VelocityProfile vel_profile(t_profile);
    return m_own_robot[t_robot_num].state().position.distanceTo(t_dest) / (vel_profile.speed * 0.5);
}

float Ai::calculateBallRobotReachTime(const int t_robot_num, const VelocityProfile t_profile)
{
    static Common::MedianFilter<float, 5> predTFilt{};

    const float tMax  = 5.0;
    float       predT = tMax;
    for (float forwT = 0; forwT < tMax; forwT += 0.02f)
    {
        Common::Vec2 newBallPos  = predictBallForwardAI(forwT);
        float        robotReachT = calculateRobotReachTime(t_robot_num, newBallPos, t_profile);
        if (robotReachT <= forwT)
        {
            predT = robotReachT;
            break;
        }
    }
    predTFilt.add(predT);

    return predTFilt.current();
}

void Ai::attacker(const int t_robot_num, const Common::Angle t_angle, const float t_kick, const int t_chip,
                  const bool t_kiss, const bool t_dribbler, const bool precise)
{
    Common::debug().draw(
        Common::LineSegment{m_world_state.ball.position, m_world_state.ball.position + t_angle.toUnitVec() * 1000.0f},
        Common::Color::blue());

    // t_kick=100;
    if (m_ref_state.restart() && (t_chip > 0))
    {
        m_chip_head = m_own_robot[t_robot_num].state().angle;
    }

    m_predicted_ball = m_world_state.ball.position;

    float d = m_own_robot[t_robot_num].state().position.distanceTo(
        m_world_state.ball.position.circleAroundPoint(t_angle, 200.0f));
    d += m_world_state.ball.position.distanceTo(m_world_state.ball.position.circleAroundPoint(t_angle, 200.0f));
    d -= 100;

    d /= 2000.0f;

    m_predicted_ball.x += d * m_world_state.ball.velocity.x / 45.0f;
    m_predicted_ball.y += d * m_world_state.ball.velocity.y / 45.0f;

    m_predicted_ball = predictBallForwardAI(d);

    Common::debug().draw(Common::Circle{m_predicted_ball, 50}, Common::Color::blue(), false);

    static bool passedBall = false;

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

    Common::Vec2 robotToBall    = m_own_robot[t_robot_num].state().position - m_world_state.ball.position;
    float        robotToBallDot = robotToBall.x * ballVelPrepToGoal.x + robotToBall.y * ballVelPrepToGoal.y;
    robotToBallDot /= robotToBall.length();
    robotToBallDot /= ballVelPrepToGoal.length();
    if (robotToBallDot > 0.3)
        passedBall = true;
    else if ((robotToBallDot < -0.2) ||
             (m_world_state.ball.position.distanceTo(m_own_robot[t_robot_num].state().position) > 1050))
        passedBall = false;

    if (passedBall)
    {
        Common::debug().draw(Common::Circle{m_own_robot[t_robot_num].state().position, 100}, Common::Color::gold(),
                             false);
    }

    if (m_world_state.ball.position.distanceTo(m_own_robot[t_robot_num].state().position) < 400)
        m_circle_reached_behind_ball = true;
    else if (m_world_state.ball.position.distanceTo(m_own_robot[t_robot_num].state().position) > 600)
        m_circle_reached_behind_ball = false;

    Common::logDebug("circle dadam");

    float r = 150.0f;

    if (t_kiss)
    {
        r = 82.0f;
    }

    float tetta = 10.0f;

    if ((t_kick > 0) || (t_chip > 0))
    {
        if (m_ref_state.restart())
        {
            r     = 200.0f;
            tetta = 20.0f;
        }
        else
        {
            r     = 1000.0f;
            tetta = 45.0f;
            // if ( !passedBall )
            //	r *= 1.5;
        }
    }

    if (t_dribbler)
    {
        if (m_own_robot[t_robot_num].state().position.distanceTo(m_world_state.ball.position) < 190)
            m_own_robot[t_robot_num].dribble(15);
    }

    if (m_ref_state.restart())
        m_own_robot[t_robot_num].face(m_predicted_ball);
    else
        m_own_robot[t_robot_num].target.angle = t_angle - Common::Angle::fromDeg(180.0f);

    // move the ball forward a bit to compensate for the over predict we do on our robots
    const Common::Vec2 ball_moved_towards_target = m_predicted_ball - t_angle.toUnitVec() * 100.0f;
    Common::Angle      hehe = ball_moved_towards_target.angleWith(m_own_robot[t_robot_num].state().position);
    hehe                    = t_angle - hehe;

    if ((std::fabs(hehe.deg()) < tetta)) //|| ( m_circle_reached_behind_ball ) )
    {
        Common::debug().draw(Common::Circle{m_own_robot[t_robot_num].state().position, 100}, Common::Color::red(),
                             false);
        if ((t_kick > 0.f) || (t_chip))
        {
            if (m_circle_reached_behind_ball)
            {
                Common::logDebug("reached");
                Common::Vec2 targetPoint;
                if (!m_ref_state.restart())
                {
                    float normalized_hehe = std::fabs(hehe.deg()) / tetta;
                    normalized_hehe       = std::pow(normalized_hehe, 0.3f);
                    targetPoint = m_predicted_ball.circleAroundPoint(t_angle, std::min(r, normalized_hehe * 265.0f));

                    if (!precise)
                    {
                        targetPoint -= t_angle.toUnitVec() * (1.0f - std::pow(normalized_hehe, 0.5f)) * 400.0f;
                    }
                }
                else
                {
                    targetPoint = m_predicted_ball.circleAroundPoint(
                        t_angle, std::min(r, std::fabs(hehe.deg()) * 320.0f / (tetta)));
                }

                // This part extends the target point towards the goal
                if (precise)
                {
                    Common::debug().draw(targetPoint, Common::Color::maroon());

                    Common::logDebug("elendil: {}", elendil);
                    Common::Angle hehe2 = m_predicted_ball.angleWith(m_own_robot[t_robot_num].state().position);
                    hehe2               = t_angle - hehe2;
                    Common::logDebug("hehe2: {}", hehe2.deg());

                    const bool el_in =
                        ((std::abs(hehe2.deg()) < 5.0f) &&
                         (m_world_state.ball.position.distanceTo(m_own_robot[t_robot_num].state().position) < 100));
                    const bool el_out =
                        ((std::abs(hehe2.deg()) > 10.0f) &&
                         (m_world_state.ball.position.distanceTo(m_own_robot[t_robot_num].state().position) > 200));

                    if (el_in)
                        elendil = 30;
                    if (el_out)
                        elendil = 0;

                    if (elendil > 0)
                    {
                        elendil--;
                        // extend towards the shoot target
                        targetPoint -= t_angle.toUnitVec() * 200.0f;
                    }
                    else
                    {
                        // extend towards the point behind the ball
                        // targetPoint += (targetPoint - m_own_robot[t_robot_num].state().position) / 2.0f;
                    }
                }

                navigate(t_robot_num, targetPoint, VelocityProfile::kharaki());
            }
            else
                navigate(
                    t_robot_num,
                    m_predicted_ball.circleAroundPoint(t_angle, std::min(r, std::fabs(hehe.deg()) * 320.0f / tetta)),
                    VelocityProfile::mamooli());
        }
        else
            navigate(t_robot_num, m_predicted_ball.circleAroundPoint(t_angle + hehe * 0.0f, r));
    }
    else
    {
        Common::debug().draw(Common::Circle{m_own_robot[t_robot_num].state().position, 100}, Common::Color::orange(),
                             false);

        hehe = m_predicted_ball.angleWith(m_own_robot[t_robot_num].state().position) +
               Common::Angle::fromDeg(Common::sign(hehe.deg()) * tetta);
        if (passedBall)
            navigate(t_robot_num, m_predicted_ball.circleAroundPoint(hehe, r));
        else
            navigate(t_robot_num, m_predicted_ball.circleAroundPoint(hehe, r), VelocityProfile::kharaki());
    }

    Common::debug().draw(Common::Circle{m_own_robot[t_robot_num].state().position, 100}, Common::Color::violet(),
                         false);
    Common::debug().draw(
        Common::LineSegment{
            m_own_robot[t_robot_num].state().position,
            m_own_robot[t_robot_num].state().position +
                (m_own_robot[t_robot_num].target.position - m_own_robot[t_robot_num].state().position).normalized() *
                    1000.0f},
        Common::Color::black());

    if ((t_kick > 0.f) || (t_chip > 0))
    {
        if (std::fabs((m_own_robot[t_robot_num].target.angle - m_own_robot[t_robot_num].state().angle).deg()) < 40)
            lockAngleCounter++;
        else
            lockAngleCounter = 0;

        // if ( lockAngleCounter > 1 )
        {
            // if ( t_chip )
            m_own_robot[t_robot_num].chip(t_chip);
            // else
            float vel_delta = m_own_robot[t_robot_num].state().velocity.length(); // convert to m/s
            vel_delta       = std::min(4000.0f, vel_delta);
            vel_delta       = t_kick - vel_delta;
            m_own_robot[t_robot_num].shoot(t_kick);
        }
    }
}
} // namespace Tyr::Soccer
