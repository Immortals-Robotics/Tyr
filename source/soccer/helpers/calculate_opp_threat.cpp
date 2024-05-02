#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::calculateOppThreat(int opp, bool restart)
{
    if (OppRobot[opp].seen_state == Common::SeenState::CompletelyOut)
        return -1;
    if (opp == oppGK)
        return -1;

    if ((OppRobot[opp].position.distanceTo(ball.position) < 400) &&
        ((OwnRobot[attack].State.position.distanceTo(ball.position) < 400) || restart))
        return -1;

    if (OppRobot[opp].position.x * side < 1000 && std::fabs(OppRobot[opp].position.x - ball.position.x) > 3000)
        return -1;

    float oppDisToGoal = OppRobot[opp].position.distanceTo(Common::Vec2(side * field_width, 0));

    Common::Vec2  t2                 = Common::Vec2(field_width * side, goal_width / 2.0f);
    Common::Vec2  t1                 = Common::Vec2(field_width * side, -goal_width / 2.0f);
    Common::Angle t1Angel            = OppRobot[opp].position.angleWith(t1);
    Common::Angle t2Angel            = OppRobot[opp].position.angleWith(t2);
    float         oppOpenAngleToGoal = std::fabs((t2Angel - t1Angel).deg());

    Common::Vec2 oppToBall = (ball.position - OppRobot[opp].position).normalized();
    Common::Vec2 oppToGoal = (Common::Vec2(side * field_width, 0) - OppRobot[opp].position).normalized();

    float oneTouchDot = oppToBall.dot(oppToGoal);

    float ballToOppDis = ball.position.distanceTo(OppRobot[opp].position);

    float score_goal_dis;
    if (oppDisToGoal < 3000)
        score_goal_dis = 1.0f;
    else
        score_goal_dis = 1.0f - pow(std::max(0.0f, (oppDisToGoal - 3000.0f) / 3000.0f), 0.5f);

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
