#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::goal_blocked(Common::Vec2 init_pos, float max_shoot_blocker_dis, float shoot_blocker_r)
{
    bool oppGoalOpen = true;

    Line ballGoalLine =
        Line::makeLineFromTwoPoints(VecPosition(-side * field_width, 0), VecPosition(init_pos.x, init_pos.y));
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OppRobot[i].seenState == Common::CompletelyOut)
            continue;
        if ((std::fabs(OppRobot[i].Position.x) > field_width) || (std::fabs(OppRobot[i].Position.y) > field_height))
            continue;
        if (OppRobot[i].Position.distanceTo(init_pos) > max_shoot_blocker_dis)
            continue;

        if (ballGoalLine.getDistanceWithPoint(VecPosition(OppRobot[i].Position.x, OppRobot[i].Position.y)) <
            shoot_blocker_r)
        {
            oppGoalOpen = false;
            break;
        }
    }
    /*if (oppGoalOpen) {
        for (int i = 0; i < 6; i ++) {
            if ( OwnRobot[i].State.seenState == Common::CompletelyOut )
                continue;
            if ( ( std::fabs ( OwnRobot[i].State.Position.x ) > 3025 ) ||
                ( std::fabs ( OwnRobot[i].State.Position.y ) > 2025 ) )
                continue;
            if (DIS(OwnRobot[i].State.Position, init_pos)>max_shoot_blocker_dis)
                continue;

            if
    (ballGoalLine.getDistanceWithPoint(VecPosition(OwnRobot[i].State.Position.x,OwnRobot[i].State.Position.y))<shoot_blocker_r)
    { oppGoalOpen = false; break;
            }
        }
    }*/

    return !oppGoalOpen;
}
} // namespace Tyr::Soccer
