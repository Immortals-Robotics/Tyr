#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::penaltyTheirSimple()
{
    float penalty_x = Common::field().width - 85.0;

        const std::optional<Common::RobotState> opp_attack = findKickerOpp();
    if (!opp_attack.has_value())
    {
        m_own_robot[m_gk].target.angle = Common::Angle::fromDeg((1 + m_side) * 90.0f);
        m_own_robot[m_gk].navigate(Common::Vec2(m_side * penalty_x, 0.0f));
    }
    else
    {
        float gkp_y = Common::Line::fromPointAndAngle(m_world_state.ball.position, opp_attack.value().angle)
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

        m_own_robot[m_gk].navigate(Common::Vec2(m_side * penalty_x, gkp_y), VelocityProfile::kharaki());
    }

    m_own_robot[m_def1].navigate(Common::Vec2(-m_side * 4300, 800), VelocityProfile::aroom());
    m_own_robot[m_def2].navigate(Common::Vec2(-m_side * 4300, -500), VelocityProfile::aroom());

    m_own_robot[m_mid1].navigate(Common::Vec2(-m_side * 4300, -800), VelocityProfile::aroom());
    m_own_robot[m_mid2].navigate(Common::Vec2(-m_side * 4300, -1500), VelocityProfile::aroom());
    m_own_robot[m_mid3].navigate(Common::Vec2(-m_side * 4300, 1500), VelocityProfile::aroom());
    m_own_robot[m_mid4].navigate(Common::Vec2(-m_side * 4300, 500), VelocityProfile::aroom());
    // TODO: navigate other robots

    m_own_robot[m_attack].navigate(Common::Vec2(-m_side * 4300, 0), VelocityProfile::aroom());
}
} // namespace Tyr::Soccer
