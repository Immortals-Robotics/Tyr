#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::tech_khers_pass(void)
{
    std::map<int, Common::vec2> rcv_pos;
    rcv_pos[dmf] = Common::vec2(-2500, 1000);
    rcv_pos[rmf] = Common::vec2(-500, 1000);
    rcv_pos[lmf] = Common::vec2(-2500, -1000);
    rcv_pos[cmf] = Common::vec2(-500, -1000);

    float min_dis      = 6000;
    int   attack_index = -1;

    int        curr_rcv = -1;
    static int last_rcv = -1;

    if (last_rcv != -1)
    {
        if (OwnRobot[last_rcv].State.seenState != Common::CompletelyOut)
        {
            if (oneTouchDetector[last_rcv].IsArriving(40, 80))
            {
                curr_rcv = last_rcv;
            }
        }
    }

    if (curr_rcv == -1)
    {
        for (std::map<int, Common::vec2>::const_iterator it = rcv_pos.begin(); it != rcv_pos.end(); it++)
        {
            if (it->first == attack_index)
            {
                continue;
            }
            if (OwnRobot[it->first].State.seenState == Common::CompletelyOut)
            {
                continue;
            }

            if (oneTouchDetector[it->first].IsArriving(40, 80))
            {
                curr_rcv = it->first;
            }
        }
    }

    static int next_rcv = -1;

    if ((next_rcv == -1) || (last_rcv != curr_rcv))
    {
        last_rcv = curr_rcv;
        for (std::map<int, Common::vec2>::const_iterator it = rcv_pos.begin(); it != rcv_pos.end(); it++)
        {
            if (it->first == attack_index)
            {
                continue;
            }
            if (OwnRobot[it->first].State.seenState == Common::CompletelyOut)
            {
                continue;
            }

            if (Common::distance(OppRobot[findNearestAsshole(it->second, -1, 1)].Position, it->second) < 500)
            {
                continue;
            }

            bool blocked     = false;
            Line to_rcv_line = Line::makeLineFromTwoPoints(VecPosition(ball.Position.x, ball.Position.y),
                                                           VecPosition(it->second.x, it->second.y));
            for (int i = 0; i < Common::Setting::kMaxRobots; i++)
            {
                if (OppRobot[i].seenState == Common::CompletelyOut)
                    continue;
                if ((fabs(OppRobot[i].Position.x) > 3025) || (fabs(OppRobot[i].Position.y) > 2025))
                    continue;

                if (to_rcv_line.getDistanceWithPoint(VecPosition(OppRobot[i].Position.x, OppRobot[i].Position.y)) < 150)
                {
                    blocked = true;
                    break;
                }
            }

            if (blocked)
            {
                continue;
            }

            next_rcv = it->first;
            break;
        }
    }

    if (Common::length(ball.velocity) < 50.0f)
    {
        for (std::map<int, Common::vec2>::const_iterator it = rcv_pos.begin(); it != rcv_pos.end(); it++)
        {
            if (OwnRobot[it->first].State.seenState == Common::CompletelyOut)
            {
                continue;
            }
            if (oneTouchDetector[it->first].IsArriving(20, 60))
            {
                continue;
            }
            if (Common::distance(it->second, ball.Position) < min_dis)
            {
                min_dis      = Common::distance(it->second, ball.Position);
                attack_index = it->first;
            }
        }
    }

    if (attack_index != -1)
    {
        Common::Angle pass_angle =
            Common::angle_with(OwnRobot[next_rcv].State.Position, OwnRobot[attack_index].State.Position);
        tech_circle(attack_index, pass_angle, 8, 0, 0, 1, 0, 0);
    }

    for (std::map<int, Common::vec2>::const_iterator it = rcv_pos.begin(); it != rcv_pos.end(); it++)
    {
        if (it->first == attack_index)
        {
            continue;
        }

        if (oneTouchDetector[it->first].IsArriving(40, 80))
        {
            Common::vec2 target =
                CalculatePassPos(it->first, OwnRobot[next_rcv].State.Position, OwnRobot[it->first].State.Position, 85);
            // OwnRobot[it->first].face(ball.Position);
            OwnRobot[it->first].face(OwnRobot[next_rcv].State.Position);
            Navigate2Point(it->first, target);
            // OwnRobot[it->first].Dribble(15);
        }
        else
        {
            Navigate2Point(it->first, it->second);
            OwnRobot[it->first].face(ball.Position);
        }
    }
}
} // namespace Tyr::Soccer
