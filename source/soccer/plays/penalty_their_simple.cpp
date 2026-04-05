#include "../ai.h"

#include "../helpers/find_kicker_opp.h"
#include "../tactics/gk.h"

namespace Tyr::Soccer
{
void Ai::penaltyTheirSimple()
{
    float penalty_x = Common::field().width - 85.0;

    if (m_ref_state.theirPenaltyPrepare())
    {
        m_own_robot[m_gk].target.angle = Common::Angle::fromDeg((1 + m_side) * 90.0f);
        m_own_robot[m_gk].navigate(Common::Vec2(m_side * penalty_x, 0.0f), VelocityProfile::mamooli(), NavigationFlags::NoOwnPenaltyArea);
    }
    else if (m_ref_state.theirPenaltyInPLay())
    {
        GkTactic{}.execute(m_own_robot[m_gk]);
    }

    m_own_robot[m_def1].navigate(Common::Vec2(-m_side * 3500, 800), VelocityProfile::aroom());
    m_own_robot[m_def2].navigate(Common::Vec2(-m_side * 3500, -500), VelocityProfile::aroom());

    m_own_robot[m_mid1].navigate(Common::Vec2(-m_side * 3500, -800), VelocityProfile::aroom());
    m_own_robot[m_mid2].navigate(Common::Vec2(-m_side * 3500, -1500), VelocityProfile::aroom());
    m_own_robot[m_mid3].navigate(Common::Vec2(-m_side * 3500, 1500), VelocityProfile::aroom());
    m_own_robot[m_mid4].navigate(Common::Vec2(-m_side * 3500, 500), VelocityProfile::aroom());
    m_own_robot[m_mid5].navigate(Common::Vec2(-m_side * 3500, 2000), VelocityProfile::aroom());
    m_own_robot[m_mid6].navigate(Common::Vec2(-m_side * 3500, -2000), VelocityProfile::aroom());
    m_own_robot[m_mid7].navigate(Common::Vec2(-m_side * 3500, 2500), VelocityProfile::aroom());

    // TODO: navigate other robots

    m_own_robot[m_attack].navigate(Common::Vec2(-m_side * 3500, 0), VelocityProfile::aroom());
}
} // namespace Tyr::Soccer
