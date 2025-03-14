#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::mark2Goal(const int t_robot_num, const int t_opp, float t_dist, bool t_def_area_mark)
{
    const float opp_predict_t = 0.15f;
    t_dist                    = std::min(1500.0f, t_dist);

    Common::Vec2 predictedOpp =
        m_world_state.opp_robot[t_opp].position + m_world_state.opp_robot[t_opp].velocity * opp_predict_t;
    Common::Vec2 target;

    if (t_def_area_mark)
    {
        const float penalty_area_dist = 120.f;
        const float penalty_area_half_width = Common::field().penalty_area_width / 2.0f;
        const Common::Vec2 start{ownGoal().x, -(penalty_area_half_width + penalty_area_dist)};
        const float        w = -m_side * (penalty_area_dist + Common::field().penalty_area_depth);
        const float        h = Common::field().penalty_area_width + 2 * penalty_area_dist;

        const Common::Rect virtual_defense_area{start, w, h};
        const Common::Line shot_line = Common::Line::fromTwoPoints(ownGoal(), predictedOpp);
        const std::vector<Common::Vec2> intersections = virtual_defense_area.intersection(shot_line);

        if (intersections.size() == 2)
        {
            target = intersections[0].distanceTo(ownGoal()) > intersections[1].distanceTo(ownGoal()) ? intersections[0]
                                                                                                : intersections[1];
        }
        // Common::debug().draw(target, Common::Color::blue());
    }
    else
    {
        target = predictedOpp.pointOnConnectingLine(ownGoal(), t_dist);
    }

    m_own_robot[t_robot_num].face(m_world_state.ball.position);
    navigate(t_robot_num, target, VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);
}
} // namespace Tyr::Soccer
