#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::normalPlayDef()
{
    Common::Vec2 ourgoal_p1 = ownGoalPostTop();
    Common::Vec2 ourgoal_p2 = ownGoalPostBottom();
    Common::debug().draw(Common::Triangle{ourgoal_p1, m_world_state.ball.position, ourgoal_p2},
                         Common::Color::blue().transparent(), true);

    int zone_idx = 0;
    for (int mid_idx = 0; mid_idx < m_prioritized_mids.size(); ++mid_idx)
    {
        int *const role = m_prioritized_mids[mid_idx];
        auto i = m_mark_map.find(role);
        int opp = i->second;
        int own = *i->first;

        if (m_own_robot[own].navigated())
        {
            Common::logWarning("robot {} is already navigated before marking opp {}", own, opp);
            continue;
        }

        if (m_own_robot[own].one_touch_detector.isArriving())
        {
            waitForPass(m_own_robot[own], false);
        }
        else if (opp == -1)
        {
            Common::Vec2 static_pos = m_sorted_zones[zone_idx]->best_pos;
            ++zone_idx;
            m_own_robot[own].face(m_world_state.ball.position);
            m_own_robot[own].navigate(static_pos, VelocityProfile::mamooli());
            m_own_robot[own].shoot(0);
        }
        else
        {
            mark(m_own_robot[own], opp, Common::config().soccer.mark_distance);
        }
    }

    OpenAngle     openAngle  = calculateOpenAngleToGoal(m_world_state.ball.position, m_own_robot[m_attack]);
    Common::Angle shootAngle = Common::Angle::fromDeg(180) + openAngle.center;

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
    else if (!goalBlocked(m_world_state.ball.position, 3000, 130))
    {
        shoot_pow = 6500.f;
        chip_pow  = 0;
    }
    else
    {
        shoot_pow = 0;
        chip_pow  = 20;
    }

    const int kicker_opp = findKickerOpp(-1, 250.0f);

    // chip the ball out if in a dangerous position
#if 1
    if (kicker_opp != -1)
    {
        shootAngle = m_world_state.ball.position.angleWith(ownGoal());
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

    attacker(m_own_robot[m_attack], shootAngle, shoot_pow, chip_pow, 0, 1);
    // circleBall(m_attack, 90, 80, 0);
}
} // namespace Tyr::Soccer
