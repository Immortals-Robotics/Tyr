#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::stop()
{
    manageAttRoles();

    gkHi(m_gk);
    defHi(m_def, m_rw, m_lw, nullptr);

    m_is_defending = Common::config().soccer.mark_in_stop;
    markManager();

    for (std::map<int *, int>::const_iterator i = m_mark_map.begin(); i != m_mark_map.end(); ++i)
    {
        int opp = i->second;
        int own = *i->first;

        if (opp == -1)
            continue;

        if (m_own_robot[own].navigated())
        {
            Common::logWarning("robot {} is already navigated before marking opp {}", own, opp);
            continue;
        }

        mark(own, opp, 500);
    }

    const std::unordered_map<int, Common::Vec2> static_poses{
        {m_dmf, m_world_state.ball.position.pointOnConnectingLine(
                    ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f)},
        {m_mid1, m_world_state.ball.position.circleAroundPoint(
                     Common::Angle::fromDeg(-20.0f) + m_world_state.ball.position.angleWith(ownGoal()), 650)},
        {m_mid2, m_world_state.ball.position.circleAroundPoint(
                     Common::Angle::fromDeg(20) + m_world_state.ball.position.angleWith(ownGoal()), 650)},
        {m_attack,
         m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 650)}};

    for (const auto static_pos : static_poses)
    {
        if (m_own_robot[static_pos.first].navigated())
        {
            continue;
        }

        m_own_robot[static_pos.first].face(m_world_state.ball.position);
        navigate(static_pos.first, static_pos.second, VelocityProfile::aroom());
        m_own_robot[static_pos.first].shoot(0);
    }
}
} // namespace Tyr::Soccer
