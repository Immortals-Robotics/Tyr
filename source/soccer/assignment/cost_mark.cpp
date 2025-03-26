#include "../ai.h"

namespace Tyr::Soccer
{
int Ai::markRoleCost(const int t_robot_idx, const Assignment &t_assignment) const
{
    const Common::RobotState &own_robot = m_own_robot[t_robot_idx].state();
    const Common::RobotState &opp_robot = m_world_state.opp_robot[t_assignment.target_idx];

    if (own_robot.seen_state == Common::SeenState::CompletelyOut)
        return -1;
    if (opp_robot.seen_state == Common::SeenState::CompletelyOut)
        return -1;
    if (t_robot_idx == m_attack)
        return -1;

    constexpr float kPredictT = 0.3f;

    const Common::Vec2 predicted_pos_own = own_robot.position + own_robot.velocity * kPredictT;
    const Common::Vec2 predicted_pos_opp = opp_robot.position + opp_robot.velocity * kPredictT;
    Common::Vec2       target;

    auto oppToBall    = (m_world_state.ball.position - predicted_pos_opp).normalized();
    auto oppToGoal    = (Field::ownGoal() - predicted_pos_opp).normalized();
    auto oppToGoalDis = predicted_pos_opp.distanceTo(Field::ownGoal());
    auto oneTouchDot  = oppToBall.dot(oppToGoal);

    if ((oneTouchDot > 0 || oppToGoalDis < 2500) && Common::config().soccer.penalty_area_mark)
    {
        const float        penalty_area_dist       = Common::config().soccer.penalty_area_mark_distance;
        const float        penalty_area_half_width = Common::field().penalty_area_width / 2.0f;
        const Common::Vec2 start{Field::ownGoal().x, -(penalty_area_half_width + penalty_area_dist)};
        const float        w = -m_side * (penalty_area_dist + Common::field().penalty_area_depth);
        const float        h = Common::field().penalty_area_width + 2 * penalty_area_dist;

        const Common::Rect              virtual_defense_area{start, w, h};
        const Common::Line              shot_line     = Common::Line::fromTwoPoints(Field::ownGoal(), predicted_pos_opp);
        const std::vector<Common::Vec2> intersections = virtual_defense_area.intersection(shot_line);

        if (intersections.size() == 2)
        {
            target = intersections[0].distanceTo(Field::ownGoal()) > intersections[1].distanceTo(Field::ownGoal()) ? intersections[0]
                                                                                                     : intersections[1];
        }
        // Common::debug().draw(target, Common::Color::blue());
    }
    else
    {
        target = predicted_pos_opp;
    }

    const float dis_pred = predicted_pos_own.distanceTo(target);

    const bool already_marked = t_robot_idx == t_assignment.currentAssignee();

    float cost_stay  = already_marked ? (dis_pred - 500.0f) / 1000.0f : 1.0f;
    float cost_reach = (dis_pred - 500.0f) / 10000.0f;

    cost_stay  = std::clamp(cost_stay, 0.0f, 1.0f);
    cost_reach = std::clamp(cost_reach, 0.0f, 1.0f);

    static constexpr float kWeightStay  = 0.0f;
    static constexpr float kWeightReach = 1.0f;

    const float cost = 10000.0f * (kWeightStay * cost_stay + kWeightReach * cost_reach);

    return cost;
}
} // namespace Tyr::Soccer
