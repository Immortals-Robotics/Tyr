#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::staticPosScoreAttack(const Common::Vec2 t_pos) const
{
    // Reject if blocks the attack's shot on goal
    const Common::Line pos_ball_line = Common::Line::fromTwoPoints(m_world_state.ball.position, t_pos);
    const Common::Line goal_line = Common::Line::fromSegment(Field::oppGoalLine());
    const auto goal_inter = pos_ball_line.intersect(goal_line);

    const float dot = (t_pos - m_world_state.ball.position).normalized().dot((Field::oppGoal() - m_world_state.ball.position).normalized());

    if (dot > 0 && goal_inter.has_value() && std::abs(goal_inter.value().y) < Common::field().goal_width / 2.0f)
        return 0.0f;

    float oppDisToGoal = t_pos.distanceTo(Field::oppGoal());

    Common::Angle t1Angel = t_pos.angleWith(Field::oppGoalPostBottom());
    Common::Angle t2Angel = t_pos.angleWith(Field::oppGoalPostTop());

    float oppOpenAngleToGoal = std::fabs((t2Angel - t1Angel).deg());

    Common::Vec2 oppToBall = (m_world_state.ball.position - t_pos).normalized();
    Common::Vec2 oppToGoal = (Field::oppGoal() - t_pos).normalized();

    float oneTouchDot = oppToBall.dot(oppToGoal);

    float score_goal_dis;
    if (oppDisToGoal < 3000)
        score_goal_dis = 1.0f;
    else
        score_goal_dis = 1.0f - pow(std::max(0.0f, (oppDisToGoal - 3000.0f) / 3000.0f), 0.5f);

    float ballToOppDis = m_world_state.ball.position.distanceTo(t_pos);

    float score_ball_dis;
    if (ballToOppDis < 2000)
        score_ball_dis = pow(ballToOppDis / 2000.0f, 2.0f);
    else if (ballToOppDis < 6000)
        score_ball_dis = 1.0f;
    else
        score_ball_dis = 1.0f - (ballToOppDis - 6000.0f) / 6000.0f;

    float score_open_angle = oppOpenAngleToGoal / 15.0f;

    float score_one_touch_angle;

    if (oneTouchDot >= 0.0f)
        score_one_touch_angle = 4 * oneTouchDot - 4 * pow(oneTouchDot, 2.0);
    else
        score_one_touch_angle = 0.0f;

    score_goal_dis        = std::min(1.0f, std::max(0.0f, score_goal_dis));
    score_ball_dis        = std::min(1.0f, std::max(0.0f, score_ball_dis));
    score_open_angle      = std::min(1.0f, std::max(0.0f, score_open_angle));
    score_one_touch_angle = std::min(1.0f, std::max(0.0f, score_one_touch_angle));

    const bool pass_angle_ok = (t_pos - m_world_state.ball.position)
                                           .normalized()
                                           .dot((Field::ownGoal() - m_world_state.ball.position).normalized()) < 0.85f;
    const float own_goal_angle_score = pass_angle_ok ? 1.0f : 0.0f;

    auto final_score_one_touch  = own_goal_angle_score * score_one_touch_angle * std::min(score_ball_dis * score_goal_dis, score_open_angle);
    auto final_score_turn_shoot = std::min(score_ball_dis * score_goal_dis, score_open_angle);



    const float score = std::max(final_score_one_touch, final_score_turn_shoot);

    return score;
}
} // namespace Tyr::Soccer
