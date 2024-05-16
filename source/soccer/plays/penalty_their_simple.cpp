#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::penalty_their_simple()
{
    float penalty_x = Common::field().width - 85.0;

    int index = findKickerOpp(-1);
    if (index == -1)
    {
        OwnRobot[gk].target.angle = Common::Angle::fromDeg((1 + side) * 90.0f);
        navigate(gk, Common::Vec2(side * penalty_x, 0.0f));
    }
    else
    {
        float gkp_y = Common::Line::fromPointAndAngle(m_world_state.ball.position, m_world_state.opp_robot[index].angle)
                          .intersect(Common::Line::fromTwoPoints(Common::Vec2(side * penalty_x, 100),
                                                                 Common::Vec2(side * penalty_x, -100)))
                          .value_or(Common::Vec2())
                          .y;
        float max_reach_y = (Common::field().goal_width / 2.0) - 50.0;
        if (max_reach_y < gkp_y)
            gkp_y = max_reach_y;
        if (-max_reach_y > gkp_y)
            gkp_y = -max_reach_y;

        OwnRobot[gk].face(m_world_state.ball.position);

        navigate(gk, Common::Vec2(side * penalty_x, gkp_y), VelocityProfile::kharaki());
    }
    navigate(lw, Common::Vec2(-side * 4300, 500), VelocityProfile::aroom());
    navigate(rw, Common::Vec2(-side * 4300, -500), VelocityProfile::aroom());
    navigate(def, Common::Vec2(-side * 4300, 800), VelocityProfile::aroom());
    navigate(dmf, Common::Vec2(-side * 4300, -800), VelocityProfile::aroom());
    navigate(mid1, Common::Vec2(-side * 4300, -1500), VelocityProfile::aroom());
    navigate(mid2, Common::Vec2(-side * 4300, 1500), VelocityProfile::aroom());
    navigate(attack, Common::Vec2(-side * 4300, 0), VelocityProfile::aroom());
}
} // namespace Tyr::Soccer