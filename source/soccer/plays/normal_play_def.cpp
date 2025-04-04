#include "../ai.h"

#include "../helpers/open_angle.h"
#include "../helpers/find_kicker_opp.h"

#include "../tactics/mark.h"
#include "../tactics/attacker.h"

namespace Tyr::Soccer
{
void Ai::normalPlayDef()
{
    Common::Vec2 ourgoal_p1 = Field::ownGoalPostTop();
    Common::Vec2 ourgoal_p2 = Field::ownGoalPostBottom();
    Common::debug().draw(Common::Triangle{ourgoal_p1, m_world_state.ball.position, ourgoal_p2},
                         Common::Color::blue().transparent(), true);

    int zone_idx = 0;
    for (int mid_idx = 0; mid_idx < m_prioritized_mids.size(); ++mid_idx)
    {
        int *const role = m_prioritized_mids[mid_idx];
        auto       i    = m_mark_map.find(role);
        int        opp  = i->second;
        int        own  = *i->first;

        if (m_own_robot[own].navigated())
        {
            Common::logWarning("robot {} is already navigated before marking opp {}", own, opp);
            continue;
        }

        if (opp == -1)
        {
            Common::Vec2 static_pos = m_sorted_zones[zone_idx]->best_pos;
            ++zone_idx;
            m_own_robot[own].face(m_world_state.ball.position);
            m_own_robot[own].navigate(static_pos, VelocityProfile::mamooli());
            m_own_robot[own].shoot(0);
        }
        else
        {
            MarkTactic{m_world_state.opp_robot[opp], Common::config().soccer.mark_distance}.execute(m_own_robot[own]);
        }
    }

    Common::Angle shootAngle = Field::oppGoal().angleWith(m_world_state.ball.position);

    float shoot_pow = 1;
    float chip_pow  = 1;
    if (m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position) > 400)
    {
        chip_pow = 1;
    }
    if (goalBlocked(m_world_state.ball.position, 200, 90))
    {
        chip_pow = 20;
    }
    else
    {
        shoot_pow = 6500.f;
        chip_pow  = 0;
    }

    // chip the ball out if in a dangerous position
#if 0
    const std::optional<Common::RobotState> kicker_opp = findKickerOpp(-1, 250.0f);
    if (kicker_opp.has_value())
    {
        shootAngle = m_world_state.ball.position.angleWith(Field::ownGoal());
        shoot_pow  = 1;
        chip_pow   = 0;
    }
#endif

#if 0
    if (kicker_opp != -1)
    {
        Common::logDebug("in m_def we trust");

        shootAngle = m_world_state.opp_robot[kicker_opp].position.angleWith(m_world_state.ball.position);
        shoot_pow  = 1;
        chip_pow   = 0;
    }
#endif

    AttackerTactic{shootAngle, true, shoot_pow, chip_pow}.execute(m_own_robot[m_attack]);
    // circleBall(m_attack, 90, 80, 0);
}
} // namespace Tyr::Soccer
