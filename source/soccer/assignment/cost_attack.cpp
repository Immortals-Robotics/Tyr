#include "../ai.h"

#include "../helpers/ball_prediction.h"

namespace Tyr::Soccer
{
int Ai::attackRoleCost(const int t_robot_idx, const Assignment &t_assignment)
{
    const float dis_to_ball = m_own_robot[t_robot_idx].state().position.distanceTo(m_world_state.ball.position);

    // check if robot is marking an opponent
    float mark_score = 1.0f;
    auto  marked_opp = std::find_if(m_mark_map.begin(), m_mark_map.end(),
                                    [&](const auto &pair) { return *pair.first == t_robot_idx; });
    if (m_is_defending && marked_opp != m_mark_map.end())
    {
        const float oppDisToBall =
            m_world_state.opp_robot[marked_opp->second].position.distanceTo(m_world_state.ball.position);

        if (oppDisToBall > 400 || dis_to_ball > 400 || m_world_state.ball.velocity.length() > 500)
        {
            mark_score = 0.1f;
        }
        else
        {
            mark_score = 0.8f;
        }
    }

    const bool ball_is_rolling = State::world().ball.velocity.length() > 500.0f;

    Common::Angle kick_angle     = (Field::oppGoal() - State::world().ball.position).toAngle();
    const float   interception_t = calculateBallRobotReachTime(m_own_robot[t_robot_idx], kick_angle,
                                                               VelocityProfile::mamooli(), 0.0f);
    float interception_score = (5.0f - interception_t) / 5.0f;
    interception_score       = std::clamp(interception_score, 0.1f, 1.0f);

    Common::debug().draw(Common::Circle{m_own_robot[t_robot_idx].state().position, 500.0f * interception_score}, Common::Color::yellow(), false, 50.0f);

    const Common::Vec2 to_ball      = State::world().ball.position - m_own_robot[t_robot_idx].state().position;

    const float ball_angle_diff =  std::fabs(State::world().ball.velocity.angleDiff(-to_ball).deg());
    float ball_direction_score = (45.0f - ball_angle_diff) / 45.0f;
    ball_direction_score       = !ball_is_rolling ? 0.0f : std::clamp(ball_direction_score, 0.1f, 1.0f);

    const Common::Line ball_line = State::world().ball.line();
    const float dis_to_ball_line = ball_line.distanceTo(m_own_robot[t_robot_idx].state().position);
    const float ball_towards_me = ball_is_rolling && ball_angle_diff < 45.f;
    ball_direction_score = (2000.0f - dis_to_ball_line) / 2000.0f;
    ball_direction_score = std::clamp(ball_direction_score, 0.1f, 1.0f);

    float dis_score = (5000.0f - dis_to_ball) / 5000.0f;
    dis_score       = std::clamp(dis_score, 0.1f, 1.0f);

    const float receive_ball_score = !ball_towards_me ? 0.0f : (ball_direction_score + dis_score) / 2.0f;

    // Calculate another score based on the robot's physical capabilities
    const auto &physical_status = Common::config().soccer.robot_physical_status[t_robot_idx];

    float caps_score = 1.0f;
    if (!physical_status.is_3D_printed)
        caps_score *= 0.8f;
    if (!physical_status.has_direct_kick && t_assignment.needs_shoot)
        caps_score *= 0.1f;
    if (!physical_status.has_chip_kick && t_assignment.needs_chip)
        caps_score *= 0.5f;

    const float role_score = t_robot_idx == m_attack ? 1.0f : 0.95f;

    // Return the combined cost
    (void) mark_score;
    const int cost = 1000.0f * (4.f - interception_score - caps_score - role_score - receive_ball_score);
    return cost;
}
} // namespace Tyr::Soccer
