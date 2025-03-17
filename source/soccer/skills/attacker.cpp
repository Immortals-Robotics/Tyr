#include "../ai.h"

namespace Tyr::Soccer
{
int lockAngleCounter = 0;
int elendil          = 0;

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
            r     = 900.0f;
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

                navigate(t_robot_num, targetPoint, VelocityProfile::kharaki(), NavigationFlagsForceNoBreak);
            }
            else
                navigate(
                    t_robot_num,
                    m_predicted_ball.circleAroundPoint(t_angle, std::min(r, std::fabs(hehe.deg()) * 320.0f / tetta)),
                    VelocityProfile::mamooli(),
                    NavigationFlagsForceNoBreak);
        }
        else
            navigate(t_robot_num, m_predicted_ball.circleAroundPoint(t_angle + hehe * 0.0f, r), VelocityProfile::mamooli(), NavigationFlagsForceNoBreak);
    }
    else
    {
        Common::debug().draw(Common::Circle{m_own_robot[t_robot_num].state().position, 100}, Common::Color::orange(),
                             false);

        hehe = m_predicted_ball.angleWith(m_own_robot[t_robot_num].state().position) +
               Common::Angle::fromDeg(Common::sign(hehe.deg()) * tetta);
        if (passedBall)
            navigate(t_robot_num, m_predicted_ball.circleAroundPoint(hehe, r), VelocityProfile::mamooli(), NavigationFlagsForceNoBreak);
        else
            navigate(t_robot_num, m_predicted_ball.circleAroundPoint(hehe, r), VelocityProfile::kharaki(), NavigationFlagsForceNoBreak);
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
