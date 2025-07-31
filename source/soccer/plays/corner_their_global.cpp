#include "../ai.h"

#include "../skills/defence_wall.h"
#include "../tactics/def.h"
#include "../tactics/gk.h"
#include "../tactics/mark.h"

namespace Tyr::Soccer
{
void Ai::cornerTheirGlobal()
{
    m_is_defending = true;

    m_assignments.clear();
    createGkAssignment();
    createDefAssignments();
    createMidAssignments();
    createAttackAssignment();
    assignRoles();

    if (m_side * m_world_state.ball.position.x > Common::field().width - 1000)
    {
        m_own_robot[m_gk].target.angle = Common::Angle::fromDeg((1 + m_side) * 90.0f);
        m_own_robot[m_gk].navigate(Common::Vec2(m_side * (Common::field().width - 100), 0), VelocityProfile::mamooli(), NavigationFlags::NoOwnPenaltyArea);
    }
    else
    {
        GkTactic{}.execute(m_own_robot[m_gk]);
    }

    DefTactic{1}.execute(m_own_robot[m_def1]);
    DefTactic{2}.execute(m_own_robot[m_def2]);

    DefenceWallSkill{}.execute(m_own_robot[m_attack]);

    std::map<int, Common::Vec2> static_pos;
    static_pos[m_mid5]  = Common::Vec2(m_side * 3500, -Common::sign(m_world_state.ball.position.y) * 1100.0f);
    static_pos[m_mid1] = Common::Vec2(m_side * 3200, 600);
    static_pos[m_mid2] = Common::Vec2(m_side * 3200, 0);

    for (std::map<int *, int>::const_iterator i = m_mark_map.begin(); i != m_mark_map.end(); ++i)
    {
        int opp = i->second;
        int own = *i->first;

        if (opp == -1)
        {
            if (static_pos.find(own) != static_pos.end())
            {
                m_own_robot[own].face(Field::oppGoal());
                m_own_robot[own].navigate(static_pos[own], VelocityProfile::mamooli());
            }
        }
        else
        {
            MarkTactic{m_world_state.opp_robot[opp], Common::config().soccer.mark_distance}.execute(m_own_robot[own]);
        }
    }
}
} // namespace Tyr::Soccer
