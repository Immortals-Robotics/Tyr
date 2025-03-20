#pragma once

namespace Tyr::Soccer
{
inline std::optional<Common::RobotState> findKickerOpp(const int t_mask = -1, const float t_max_dis = 500.0f)
{
    float mdis  = t_max_dis;
    int   index = -1;
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        if (i == t_mask)
            continue;
        if (State::world().opp_robot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if (State::world().ball.position.distanceTo(State::world().opp_robot[i].position) < mdis)
        {
            mdis  = State::world().ball.position.distanceTo(State::world().opp_robot[i].position);
            index = i;
        }
    }

    if (index == -1)
    {
        return std::nullopt;
    }

    return State::world().opp_robot[index];
}
} // namespace Tyr::Soccer
