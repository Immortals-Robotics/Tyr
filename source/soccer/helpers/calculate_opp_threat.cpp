#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::calculateOppThreat(int opp, bool restart)
{
    if (Common::worldState().opp_robot[opp].seen_state == Common::SeenState::CompletelyOut)
        return -1;
    if (opp == Common::refereeState().opp_gk)
        return -1;

    if ((Common::worldState().opp_robot[opp].position.distanceTo(Common::worldState().ball.position) < 400) &&
        ((OwnRobot[attack].State.position.distanceTo(Common::worldState().ball.position) < 400) || restart))
        return -1;

    if (Common::worldState().opp_robot[opp].position.x * side < 1000 && std::fabs(Common::worldState().opp_robot[opp].position.x - Common::worldState().ball.position.x) > 3000)
        return -1;

    float oppDisToGoal = Common::worldState().opp_robot[opp].position.distanceTo(Common::Vec2(side * Common::worldState().field.width, 0));

    Common::Vec2  t2                 = Common::Vec2(Common::worldState().field.width * side, Common::worldState().field.goal_width / 2.0f);
    Common::Vec2  t1                 = Common::Vec2(Common::worldState().field.width * side, -Common::worldState().field.goal_width / 2.0f);
    Common::Angle t1Angel            = Common::worldState().opp_robot[opp].position.angleWith(t1);
    Common::Angle t2Angel            = Common::worldState().opp_robot[opp].position.angleWith(t2);
    float         oppOpenAngleToGoal = std::fabs((t2Angel - t1Angel).deg());

    Common::Vec2 oppToBall = (Common::worldState().ball.position - Common::worldState().opp_robot[opp].position).normalized();
    Common::Vec2 oppToGoal = (Common::Vec2(side * Common::worldState().field.width, 0) - Common::worldState().opp_robot[opp].position).normalized();

    float oneTouchDot = oppToBall.dot(oppToGoal);

    float ballToOppDis = Common::worldState().ball.position.distanceTo(Common::worldState().opp_robot[opp].position);

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
