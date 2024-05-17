#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::penaltyTheirSimple()
{
    float penalty_x = Common::field().width - 85.0;

    int index = findKickerOpp(-1);
    if (index == -1)
    {
        m_own_robot[m_gk].target.angle = Common::Angle::fromDeg((1 + m_side) * 90.0f);
        navigate(m_gk, Common::Vec2(m_side * penalty_x, 0.0f));
    }
    else
    {
        float gkp_y = Common::Line::fromPointAndAngle(m_world_state.ball.position, m_world_state.opp_robot[index].angle)
                          .intersect(Common::Line::fromTwoPoints(Common::Vec2(m_side * penalty_x, 100),
                                                                 Common::Vec2(m_side * penalty_x, -100)))
                          .value_or(Common::Vec2())
                          .y;
        float max_reach_y = (Common::field().goal_width / 2.0) - 50.0;
        if (max_reach_y < gkp_y)
            gkp_y = max_reach_y;
        if (-max_reach_y > gkp_y)
            gkp_y = -max_reach_y;

        m_own_robot[m_gk].face(m_world_state.ball.position);

        navigate(m_gk, Common::Vec2(m_side * penalty_x, gkp_y), VelocityProfile::kharaki());
    }
    navigate(m_lw, Common::Vec2(-m_side * 4300, 500), VelocityProfile::aroom());
    navigate(m_rw, Common::Vec2(-m_side * 4300, -500), VelocityProfile::aroom());
    navigate(m_def, Common::Vec2(-m_side * 4300, 800), VelocityProfile::aroom());
    navigate(m_dmf, Common::Vec2(-m_side * 4300, -800), VelocityProfile::aroom());
    navigate(m_mid1, Common::Vec2(-m_side * 4300, -1500), VelocityProfile::aroom());
    navigate(m_mid2, Common::Vec2(-m_side * 4300, 1500), VelocityProfile::aroom());
    navigate(m_attack, Common::Vec2(-m_side * 4300, 0), VelocityProfile::aroom());
}
} // namespace Tyr::Soccer
