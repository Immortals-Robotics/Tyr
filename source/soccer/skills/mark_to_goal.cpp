#include "mark_to_goal.h"

#include "../robot/robot.h"

namespace Tyr::Soccer
{
const Skill::Id MarkToGoalSkill::kId = &MarkToGoalSkill::kId;

void MarkToGoalSkill::execute(Robot &t_robot)
{
    const float opp_predict_t = 0.15f;

    Common::Vec2 predictedOpp = m_opp.position + m_opp.velocity * opp_predict_t;
    Common::Vec2 target;

    if (m_def_area_mark)
    {
        const float        penalty_area_dist       = Common::config().soccer.penalty_area_mark_distance;
        const float        penalty_area_half_width = Common::field().penalty_area_width / 2.0f;
        const Common::Vec2 start{Field::ownGoal().x, -(penalty_area_half_width + penalty_area_dist)};
        const float        w = -State::side() * (penalty_area_dist + Common::field().penalty_area_depth);
        const float        h = Common::field().penalty_area_width + 2 * penalty_area_dist;

        const Common::Rect              virtual_defense_area{start, w, h};
        const Common::Line              shot_line     = Common::Line::fromTwoPoints(Field::ownGoal(), predictedOpp);
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
        target = predictedOpp.pointOnConnectingLine(Field::ownGoal(), m_dist);
    }

    t_robot.face(State::world().ball.position);
    t_robot.navigate(target, VelocityProfile::mamooli(), NavigationFlags::ForceBallObstacle);
}
} // namespace Tyr::Soccer
