#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::goalBlocked(Common::Vec2 init_pos, float max_shoot_blocker_dis, float shoot_blocker_r)
{
    bool oppGoalOpen = true;

    Common::Line ballGoalLine = Common::Line::fromTwoPoints(Common::Vec2(-side * Common::field().width, 0), init_pos);
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_state.opp_robot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if ((std::fabs(m_state.opp_robot[i].position.x) > Common::field().width) || (std::fabs(m_state.opp_robot[i].position.y) > Common::field().height))
            continue;
        if (m_state.opp_robot[i].position.distanceTo(init_pos) > max_shoot_blocker_dis)
            continue;

        if (ballGoalLine.distanceTo(m_state.opp_robot[i].position) < shoot_blocker_r)
        {
            oppGoalOpen = false;
            break;
        }
    }
    /*if (oppGoalOpen) {
        for (int i = 0; i < 6; i ++) {
            if ( OwnRobot[i].state().seen_state == Common::SeenState::CompletelyOut )
                continue;
            if ( ( std::fabs ( OwnRobot[i].state().position.x ) > 3025 ) ||
                ( std::fabs ( OwnRobot[i].state().position.y ) > 2025 ) )
                continue;
            if (DIS(OwnRobot[i].state().position, init_pos)>max_shoot_blocker_dis)
                continue;

            if
    (ballGoalLine.distanceTo(Common::Vec2(OwnRobot[i].state().position.x,OwnRobot[i].state().position.y))<shoot_blocker_r)
    { oppGoalOpen = false; break;
            }
        }
    }*/

    return !oppGoalOpen;
}
} // namespace Tyr::Soccer
