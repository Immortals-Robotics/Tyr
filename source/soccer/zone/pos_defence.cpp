#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::staticPosScoreDefence(const Common::Vec2 t_pos) const
{
    float oppDisToGoal = t_pos.distanceTo(Field::ownGoal());

    Common::Angle t1Angel = t_pos.angleWith(Field::ownGoalPostBottom());
    Common::Angle t2Angel = t_pos.angleWith(Field::ownGoalPostTop());

    float oppOpenAngleToGoal = std::fabs((t2Angel - t1Angel).deg());

    Common::Vec2 oppToBall = (m_world_state.ball.position - t_pos).normalized();
    Common::Vec2 oppToGoal = (Field::ownGoal() - t_pos).normalized();

    float oneTouchDot = oppToBall.dot(oppToGoal);

    float score_goal_dis;
    if (oppDisToGoal < 3000)
        score_goal_dis = 1.0f;
    else
        score_goal_dis = 1.0f - pow(std::max(0.0f, (oppDisToGoal - 3000.0f) / 3000.0f), 0.5f);

    float ballToOppDis = m_world_state.ball.position.distanceTo(t_pos);

    if (ballToOppDis < 400)
        return -1;

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

    auto final_score_one_touch  = score_one_touch_angle * std::min(score_ball_dis * score_goal_dis, score_open_angle);
    auto final_score_turn_shoot = std::min(score_ball_dis * score_goal_dis, score_open_angle);

    auto score = std::max(final_score_one_touch, final_score_turn_shoot);

    return score;
}
} // namespace Tyr::Soccer
